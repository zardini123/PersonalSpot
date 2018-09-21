from flask import Flask, request
app = Flask(__name__)

@app.route('/', methods=["GET"])
def hello():
    return "Hello World!"

@app.route('/', methods=['POST'])
def post_file():

    # a multidict containing POST data
    # return type of werkzeug.datastructures.ImmutableMultiDict
    # https://stackoverflow.com/questions/37004983/what-exactly-is-werkzeug
    data = request.form;
    print(request.form)

    # requests.files return type same of data
    files = request.files;
    print(files)

    indieFile = files['fileFromClient']
    print(type(indieFile))

    # Save data stored in FileStorage type onto the harddrive
    indieFile.save("testFile.mp3")

    # Return 201 CREATED
    return '', 201

if __name__ == '__main__':
    app.run(debug=True, port=5000)
