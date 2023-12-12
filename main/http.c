
#include "http.h"

static const char *TAG = "HTTP_CLIENT";
esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        ESP_LOGI(TAG,"HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;
    case HTTP_EVENT_ON_FINISH:
        ESP_LOGI(TAG,"HTTP_POST_FINISH\n");
        break;
    case HTTP_EVENT_ON_CONNECTED:
        ESP_LOGI(TAG,"HTTP_CONNECTED\n");
        break;
    case HTTP_EVENT_DISCONNECTED:
        ESP_LOGI(TAG,"HTTP_DISCONNECTED\n");
        break;
    default:
        break;
    }
    return ESP_OK;
}


void Send_data_to_cloud(float data,char* topic)
{
    char post_data[100];
    char output_buffer[100];
    esp_http_client_config_t config_post = {
        .url = "http://demo.thingsboard.io/api/v1/f5jllvxrrrxhiy8sce51/telemetry",
        .method = HTTP_METHOD_POST,
        .cert_pem = NULL,
        .event_handler = client_event_post_handler};
        
    esp_http_client_handle_t client = esp_http_client_init(&config_post);

    sprintf(post_data,"{\"%s\":%.2f}",topic,data);
    esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_http_client_set_header(client, "Content-Type", "application/json");

    esp_http_client_perform(client);

    int data_read = esp_http_client_read_response(client, output_buffer, MAX_HTTP_OUTPUT_BUFFER);
    if (data_read >= 0) {
        ESP_LOGI(TAG, "HTTP POST Status = %d, content_length = %lld",
        esp_http_client_get_status_code(client),
        esp_http_client_get_content_length(client));
        ESP_LOG_BUFFER_HEX(TAG, output_buffer, strlen(output_buffer));
    } else {
        ESP_LOGE(TAG, "Failed to read response");
    }

    esp_http_client_cleanup(client);

}
