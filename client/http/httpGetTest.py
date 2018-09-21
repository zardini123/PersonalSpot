import requests

port = 5000
url = "http://localhost:%s" %port
r = requests.get(url)

print(r.text)

# data that can be sent along with the file
#payload = {'trashToSendAlongWith': "owo whats this"}
payload = [('trashToSendAlongWith', "owo whats this"), ('moreTrash', "eggs d")]

file = open('../haloTheme.mp3','rb');
files = {'fileFromClient': file}
r = requests.post(url, files=files, data=payload)
