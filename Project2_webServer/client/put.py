#!/usr/bin/python3
import requests

method = input("Enter the method (GET, POST, PUT, DELETE): ")
file = input("Enter the file name: ")
url = "http://localhost:8080/" + file
headers = {"Content-Type": "application/json"}
data = {
    "name": "Updated Name",
    "email": "updated.email@example.com"
}

response = requests.put(url, json=data, headers=headers)
print("Status Code:", response.status_code)
print("Response Body:", response.json())
