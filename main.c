#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "include/clib_openiap.h"

#define INPUT_SIZE 256

void print_help() {
    printf("Available commands:\n");
    printf("  ?         : Help\n");
    printf("  connect   : Reconnect to server\n");
    printf("  info      : Log an info message\n");
    printf("  warn      : Log a warning message\n");
    printf("  error     : Log an error message\n");
    printf("  o         : Toggle observable gauge 'test_f64'\n");
    printf("  o2        : Toggle observable gauge 'test_u64'\n");
    printf("  o3        : Toggle observable gauge 'test_i64'\n");
    printf("  q         : Execute a query on 'entities' collection\n");
    printf("  di        : Get distinct values from 'entities' collection\n");
    printf("  i         : Insert one document\n");
    printf("  im        : Insert multiple documents\n");
    printf("  w         : Watch for changes in entities collection (async)\n");
    printf("  uw        : Unwatch entities collection\n");
    printf("  r         : Register queue 'test2queue'\n");
    printf("  m         : Send message to queue 'test2queue'\n");
    printf("  cc        : Call custom_command 'getclients'\n");
    printf("  rpa       : Invoke \"Who am I\" on robot \"allan5\" \n");
    printf("  quit      : Exit the CLI\n");
}

char* active_watch_id = NULL; // Keep track of active watch

// Callback function for watch events
void watch_event_callback(struct WatchEventWrapper* event) {
    printf("\nWatch event received:\n");
    printf("  Operation: %s\n", event->operation);
    printf("  Document: %s\n", event->document);
    printf("> "); // Reprint prompt after event
    fflush(stdout);
}

// Define a callback to handle the initial watch response
void watch_response_callback(struct WatchResponseWrapper* resp) {
    if (resp != NULL) {
        if (!resp->success) {
            printf("Watch failed: %s\n", resp->error);
        } else {
            active_watch_id = strdup(resp->watchid);
            printf("Watch created with id: %s\n", active_watch_id);
        }
        free_watch_response(resp);
    }
}

// Callback for queue events
const char *queue_event_callback(struct QueueEventWrapper *event) {
    printf("\nQueue event received on queue: %s\n", event->queuename);
    printf("  Data: %s\n", event->data);
    printf("  Correlation ID: %s\n", event->correlation_id ? event->correlation_id : "(none)");
    printf("  ReplyTo: %s\n", event->replyto ? event->replyto : "(none)");
    printf("> ");
    fflush(stdout);
    // No reply
    return "{\"status\":\"ok\"}";
}

int main(void) {
    char input[INPUT_SIZE];
    bool f64_gauge_active = false;
    bool u64_gauge_active = false;
    bool i64_gauge_active = false;

    struct ClientWrapper *client = create_client();
    if (client == NULL) {
        fprintf(stderr, "Error: Failed to create client.\n");
        return 1;
    }
    
    const char *server_address = "";
    struct ConnectResponseWrapper *conn_resp = client_connect(client, server_address);
    if (conn_resp == NULL) {
        fprintf(stderr, "Error: client_connect returned NULL.\n");
        return 1;
    }
    
    if (!conn_resp->success) {
        fprintf(stderr, "Connection failed: %s\n", conn_resp->error);
        return 1;
    } else {
        printf("Connected successfully! Request ID: %d\n", conn_resp->request_id);
    }
    free_connect_response(conn_resp);

    print_help();

    while (1) {
        printf("> ");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "quit") == 0) {
            break;
        } else if (strcmp(input, "?") == 0) {
            print_help();
        } else if (strcmp(input, "connect") == 0) {
            conn_resp = client_connect(client, server_address);
            if (conn_resp == NULL) {
                printf("Error: client_connect returned NULL.\n");
            } else if (!conn_resp->success) {
                printf("Connection failed: %s\n", conn_resp->error);
            } else {
                printf("Connected successfully! Request ID: %d\n", conn_resp->request_id);
            }
            free_connect_response(conn_resp);
        } else if (strcmp(input, "info") == 0) {
            info("This is an info message from the CLI.");
        } else if (strcmp(input, "warn") == 0) {
            warn("This is a warning message from the CLI.");
        } else if (strcmp(input, "error") == 0) {
            error("This is an error message from the CLI.");
        } else if (strcmp(input, "o") == 0) {
            if (!f64_gauge_active) {
                int random_value = rand() % 50 + 1;  // random number between 1 and 50
                set_f64_observable_gauge("test_f64", (double)random_value, "test observable gauge");
                printf("Observable gauge 'test_f64' set to %d.\n", random_value);
                f64_gauge_active = true;
            } else {
                // Disable the observable gauge.
                disable_observable_gauge("test_f64");
                printf("Observable gauge 'test_f64' disabled.\n");
                f64_gauge_active = false;
            }
        } else if (strcmp(input, "o2") == 0) {
            if (!u64_gauge_active) {
                int random_value = rand() % 50 + 1;  // random number between 1 and 50
                set_u64_observable_gauge("test_u64", (uint64_t)random_value, "test observable gauge");
                printf("Observable gauge 'test_u64' set to %d.\n", random_value);
                u64_gauge_active = true;
            } else {
                // Disable the observable gauge.
                disable_observable_gauge("test_u64");
                printf("Observable gauge 'test_u64' disabled.\n");
                u64_gauge_active = false;
            }
        } else if (strcmp(input, "o3") == 0) {
            if (!i64_gauge_active) {
                int random_value = rand() % 50 + 1;  // random number between 1 and 50
                set_i64_observable_gauge("test_i64", (int64_t)random_value, "test observable gauge");
                printf("Observable gauge 'test_i64' set to %d.\n", random_value);
                i64_gauge_active = true;
            } else {
                // Disable the observable gauge.
                disable_observable_gauge("test_i64");
                printf("Observable gauge 'test_i64' disabled.\n");
                i64_gauge_active = false;
            }
        } else if (strcmp(input, "q") == 0) {
            // Build a query request for the "entities" collection.
            QueryRequestWrapper req;
            req.collectionname = "entities";
            req.query = "{}";
            req.projection = "{ \"name\": 1 }";
            req.orderby = NULL;
            req.queryas = NULL;
            req.explain = false;
            req.skip = 0;
            req.top = 0;
            req.request_id = 1; // sample request id

            struct QueryResponseWrapper *query_resp = query(client, &req);
            if (query_resp == NULL) {
                printf("Error: query returned NULL.\n");
            } else {
                if (!query_resp->success) {
                    printf("Query failed: %s\n", query_resp->error);
                } else {
                    printf("Query succeeded. Results: %s\n", query_resp->results);
                }
                free_query_response(query_resp);
            }
        } else if (strcmp(input, "di") == 0) {
            struct DistinctRequestWrapper req = {
                .collectionname = "entities",
                .field = "_type",
                .query = NULL,
                .queryas = NULL,
                .explain = false,
                .request_id = 1
            };

            struct DistinctResponseWrapper *resp = distinct(client, &req);
            if (resp == NULL) {
                printf("Error: distinct returned NULL.\n");
            } else {
                if (!resp->success) {
                    printf("Distinct failed: %s\n", resp->error);
                } else {
                    printf("Distinct values:\n");
                    for (int i = 0; i < resp->results_len; i++) {
                        printf("  %s\n", resp->results[i]);
                    }
                }
                free_distinct_response(resp);
            }
        } else if (strcmp(input, "i") == 0) {
            struct InsertOneRequestWrapper req = {
                .collectionname = "entities",
                .item = "{\"name\":\"Allan\", \"_type\":\"test\"}",
                .w = 0,
                .j = false,
                .request_id = 1
            };

            struct InsertOneResponseWrapper *resp = insert_one(client, &req);
            if (resp == NULL) {
                printf("Error: insert_one returned NULL.\n");
            } else {
                if (!resp->success) {
                    printf("Insert failed: %s\n", resp->error);
                } else {
                    printf("Insert succeeded. Result: %s\n", resp->result);
                }
                free_insert_one_response(resp);
            }
        } else if (strcmp(input, "im") == 0) {
            struct InsertManyRequestWrapper req = {
                .collectionname = "entities",
                .items = "[{\"name\":\"Allan\", \"_type\":\"test\"}, {\"name\":\"Allan2\", \"_type\":\"test\"}]",
                .w = 0,
                .j = false,
                .skipresults = false,
                .request_id = 1
            };

            struct InsertManyResponseWrapper *resp = insert_many(client, &req);
            if (resp == NULL) {
                printf("Error: insert_many returned NULL.\n");
            } else {
                if (!resp->success) {
                    printf("Insert many failed: %s\n", resp->error);
                } else {
                    printf("Insert many succeeded. Results: %s\n", resp->results);
                }
                free_insert_many_response(resp);
            }
        } else if (strcmp(input, "w") == 0) {
            if (active_watch_id != NULL) {
                printf("Watch already active. Use 'uw' to unwatch first.\n");
                continue;
            }

            struct WatchRequestWrapper req = {
                .collectionname = "entities",
                .paths = NULL,  // Changed from "[]" to NULL since empty paths are handled in Rust
                .request_id = 1
            };

            watch_async_async(client, &req, 
                watch_response_callback,  // Handle the initial response
                watch_event_callback      // Handle subsequent events
            );
            
            printf("Watch request sent...\n");
        } else if (strcmp(input, "uw") == 0) {
            if (active_watch_id != NULL) {
                struct UnWatchResponseWrapper *resp = unwatch(client, active_watch_id);
                if (resp != NULL) {
                    if (!resp->success) {
                        printf("Unwatch failed: %s\n", resp->error);
                    } else {
                        printf("Unwatched successfully\n");
                    }
                    free_unwatch_response(resp);
                }
                free(active_watch_id);
                active_watch_id = NULL;
            } else {
                printf("No active watch to unsubscribe from\n");
            }
        } else if (strcmp(input, "r") == 0) {
            struct RegisterQueueRequestWrapper req = {
                .queuename = "test2queue",
                .request_id = 1
            };
            struct RegisterQueueResponseWrapper *resp = register_queue_async(client, &req, queue_event_callback);
            if (resp == NULL) {
                printf("Error: register_queue_async returned NULL.\n");
            } else {
                if (!resp->success) {
                    printf("Register queue failed: %s\n", resp->error);
                } else {
                    printf("Registered queue as: %s\n", resp->queuename);
                }
                free_register_queue_response(resp);
            }
        } else if (strcmp(input, "r2") == 0) {
            struct QueueMessageRequestWrapper req = {
                .queuename = "test2queue",
                .striptoken = true,
                .data = "{\"message\":\"Test message\"}",
                .request_id = 1
            };

            struct RpcResponseWrapper *response = rpc(client, &req, 5);
            if (response == NULL) {
                error("RPC test failed: response is NULL.");
            } else if (!response->success) {
                error("RPC test failed:");
                error(response->error);
            } else {
                info("Received reply:");
                info(response->result);
            }
            free_rpc_response(response);
        } else if (strcmp(input, "m") == 0) {
            struct QueueMessageRequestWrapper req = {
                .queuename = "test2queue",
                .correlation_id = NULL,
                .replyto = NULL,
                .routingkey = NULL,
                .exchangename = NULL,
                .data = "{\"message\":\"Test message\"}",
                .striptoken = true,
                .expiration = 0,
                .request_id = 1
            };
            struct QueueMessageResponseWrapper *resp = queue_message(client, &req);
            if (resp == NULL) {
                printf("Error: queue_message returned NULL.\n");
            } else {
                if (!resp->success) {
                    printf("Queue message failed: %s\n", resp->error);
                } else {
                    printf("Queued message to test2queue\n");
                }
                free_queue_message_response(resp);
            }
        } else if (strcmp(input, "cc") == 0) {
            struct CustomCommandRequestWrapper req;
            req.command = "getclients";
            req.id = NULL;
            req.name = NULL;
            req.data = NULL;
            req.request_id = 1;
            struct CustomCommandResponseWrapper *resp = custom_command(client, &req, 2);
            if (resp == NULL) {
                printf("Error: custom_command returned NULL.\n");
            } else {
                if (!resp->success) {
                    printf("Custom command failed: %s\n", resp->error);
                } else {
                    printf("Custom command result: %s\n", resp->result);
                }
                free_custom_command_response(resp);
            }
        } else if (strcmp(input, "rpa") == 0) {
            struct InvokeOpenRPARequestWrapper req;
            req.robotid = "5ce94386320b9ce0bc2c3d07";
            req.workflowid = "5e0b52194f910e30ce9e3e49";
            req.payload = "{\"test\":\"test\"}";
            req.rpc = true;
            struct InvokeOpenRPAResponseWrapper *resp = invoke_openrpa(client, &req, 10);
            if (resp == NULL) {
                printf("Error: invoke_openrpa returned NULL.\n");
            } else {
                if (!resp->success) {
                    printf("Invoke OpenRPA failed: %s\n", resp->error);
                } else {
                    printf("Invoke OpenRPA result: %s\n", resp->result);
                }
                free_invoke_openrpa_response(resp);
            }
        } else if (strcmp(input, "g") == 0) {
            const char *state = client_get_state(client);
            if (state != NULL) {
                info("State:");
                info(state);
            } else {
                error("Failed to get state.");
            }

            int32_t timeout = client_get_default_timeout(client);
            info("Default timeout:");
            printf("%d seconds\n", timeout);

            client_set_default_timeout(client, 2);
            timeout = client_get_default_timeout(client);
            if (timeout == 2) {
                info("Default timeout set to 2 seconds.");
            } else {
                error("Failed to set default timeout.");
            }
        } else {
            printf("Unknown command: '%s'\n", input);
        }
    }

    if (active_watch_id != NULL) {
        unwatch(client, active_watch_id);
        free(active_watch_id);
    }

    client_disconnect(client);
    free_client(client);
    
    printf("Exiting CLI.\n");
    return 0;
}
