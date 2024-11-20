import shlex
import socket
import webbrowser
import os
import base64

import platform

import subprocess
from urllib.parse import urlparse


def parse_curl_command(curl_command):
    """
    Parses a curl command into its components and converts it into an HTTP request.
    """
    args = shlex.split(curl_command)

    method = "GET"
    url = None
    headers = {}
    data = None
    auth = None

    for i, arg in enumerate(args):
        if arg == "curl":
            continue
        elif arg.startswith("http"):
            url = arg
        elif arg == "-X":
            method = args[i + 1]
        elif arg == "-H":
            header = args[i + 1].split(": ", 1)
            headers[header[0]] = header[1]
        elif arg == "-d":
            data = args[i + 1]
        elif arg == "-u":
            auth = args[i + 1]

    return method, url, headers, data, auth


def build_http_request(method, host, path, headers, data, auth):
    """
    Builds a raw HTTP request string.
    """
    request_line = f"{method} {path} HTTP/1.1\r\n"
    header_lines = f"Host: {host}\r\n"

    # Add Basic Authentication if provided
    if auth:
        auth_encoded = base64.b64encode(auth.encode()).decode()
        header_lines += f"Authorization: Basic {auth_encoded}\r\n"

    # Add other headers
    for key, value in headers.items():
        header_lines += f"{key}: {value}\r\n"

    # Add Content-Length if data is provided
    if data:
        header_lines += f"Content-Length: {len(data)}\r\n"

    # End headers section
    header_lines += "\r\n"

    # Combine request line, headers, and body (if any)
    return request_line + header_lines + (data if data else "")


def send_request_via_socket(host, port, raw_request):
    """
    Sends the raw HTTP request via a client socket and receives the response.
    """
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.connect((host, port))
            client_socket.sendall(raw_request.encode())  # Send the HTTP request

            # Receive the response
            response = b""
            while True:
                chunk = client_socket.recv(4096)
                if not chunk:
                    break
                response += chunk

            return response
    except Exception as e:
        return f"An error occurred: {e}"


def open_in_default_app(path):
    if not path or len(path) == 0:
        print("Invalid file or URL.")
        return

    try:
        if platform.system() == 'Windows':
            print("im here")
            os.startfile(path)
        elif platform.system() == 'Darwin':  # macOS
            subprocess.run(['open', path])
        elif platform.system() == 'Linux':
            subprocess.run(['xdg-open', path])
        else:
            print("Unsupported OS")
    except Exception as e:
        print(f"Failed to open: {path}\nError: {e}")


def handle_response(response):
    """
    Handles the server's response, printing it and opening URLs or images in the web browser.
    """
    # Decode response into headers and body
    try:
        headers, body = response.split(b"\r\n\r\n", 1)
        headers_decoded = headers.decode()
    except Exception as e:
        print(f"Error parsing response: {e}")
        return

    print("Server Response Headers:")
    print(headers_decoded)

    # Check for content type
    content_type = None
    for line in headers_decoded.split("\r\n"):
        if line.lower().startswith("content-type:"):
            content_type = line.split(":", 1)[1].strip()

    if not content_type:
        print("Content-Type not found in response.")
        return

    # Determine file extension based on content type
    file_extension = content_type.split("/")[-1]
    file_path = f"response.{file_extension}"

    # Save the response body to a file
    with open(file_path, "wb") as f:
        f.write(body)
    print(f"Response saved to {file_path}")

    open_file = input("Would you like to open the file? (y/n): ")
    if open_file.lower() == "y":
        open_in_default_app(file_path)


def main():
    # Input curl command
    curl_command = input("Enter a curl command: ")

    # Parse the curl command
    method, url, headers, data, auth = parse_curl_command(curl_command)

    if not url:
        print("Error: No URL found in the curl command.")
        return

    # Parse URL into host and path
    parsed_url = urlparse(url)
    host = parsed_url.hostname
    port = parsed_url.port if parsed_url.port else 80  # Default to port 80
    path = parsed_url.path if parsed_url.path else "/"
    if parsed_url.query:
        path += f"?{parsed_url.query}"

    # Build the raw HTTP request
    raw_request = build_http_request(method, host, path, headers, data, auth)

    # Send the HTTP request via socket and get the response
    response = send_request_via_socket(host, port, raw_request)

    # Handle the server's response
    if isinstance(response, bytes):
        handle_response(response)
    else:
        print(response)


if __name__ == "__main__":
    main()
