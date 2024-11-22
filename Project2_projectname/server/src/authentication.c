#include "authentication.h"

#include <stdint.h>
#define USERS_FILE "config/users.txt"

static const char base64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

char* base64_decode(const char *input)
{
    size_t input_length = strlen(input);
    input_length--; //as there is \n which is not needed

    if (input_length % 4 != 0)
    {
        return NULL; // Invalid Base64 input length
    }

    size_t output_length = input_length / 4 * 3;
    if (input[input_length - 1] == '=') 
        output_length--;
    if (input[input_length - 2] == '=')
        output_length--;

    char *output = (char*)malloc(output_length + 1); // +1 for null terminator
    if (output == NULL)
    {
        return "memoryfailed"; // Memory allocation failed
    }

    for (size_t i = 0, j = 0; i < input_length;)
    {
        uint32_t sextet_a = input[i] == '=' ? 0 & i++ : strchr(base64_table, input[i++]) - base64_table;
        uint32_t sextet_b = input[i] == '=' ? 0 & i++ : strchr(base64_table, input[i++]) - base64_table;
        uint32_t sextet_c = input[i] == '=' ? 0 & i++ : strchr(base64_table, input[i++]) - base64_table;
        uint32_t sextet_d = input[i] == '=' ? 0 & i++ : strchr(base64_table, input[i++]) - base64_table;

        uint32_t triple = (sextet_a << 3 * 6)
                        + (sextet_b << 2 * 6)
                        + (sextet_c << 1 * 6)
                        + (sextet_d << 0 * 6);

        if (j < output_length) output[j++] = (triple >> 2 * 8) & 0xFF;
        if (j < output_length) output[j++] = (triple >> 1 * 8) & 0xFF;
        if (j < output_length) output[j++] = (triple >> 0 * 8) & 0xFF;
    }

    output[output_length] = '\0'; // Null-terminate the output string

    return output;
}

int authenticate_request(const char auth_header[])
{
    if (auth_header == NULL || strncmp(auth_header, "Basic ", 6) != 0)
    {
        return 0;
    }

    const char *encoded_credentials = auth_header + 6;
    const char *end = strchr(encoded_credentials, '\n');
    if (end)
    {
        encoded_credentials = strndup(encoded_credentials, end - encoded_credentials);
    }

    char* decoded_credentials;
    decoded_credentials = base64_decode(encoded_credentials);

    if(decoded_credentials == NULL)
    {
        return 0;
    }
    else if(strcmp(decoded_credentials, "memoryfailed") == 0)
    {
        return 2;
    }

    FILE *file = fopen(USERS_FILE, "r");
    if (!file)
    {
        return 3;
    }

    char line[256];
    int authenticated = 0;
    while (fgets(line, sizeof(line), file))
    {
        line[strcspn(line, "\n")] = '\0'; // Trim newline
        if (strcmp(decoded_credentials, line) == 0)
        {
            authenticated = 1;
            break;
        }
    }

    free(decoded_credentials);
    fclose(file);
    return authenticated;
}
