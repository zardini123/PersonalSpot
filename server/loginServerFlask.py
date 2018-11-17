# https://www.owasp.org/index.php/Password_Storage_Cheat_Sheet


@app.route('/login/', methods=["GET","POST"])
def login_page():
    error = ''
    try:
        c, conn = connection()
        if request.method == "POST":

            data = c.execute("SELECT * FROM users WHERE username = (%s)",
                             thwart(request.form['username']))

            data = c.fetchone()[2]

            if sha256_crypt.verify(request.form['password'], data):
                session['logged_in'] = True
                session['username'] = request.form['username']

                flash("You are now logged in")
                return redirect(url_for("dashboard"))

            else:
                error = "Invalid credentials, try again."

        gc.collect()

        return render_template("login.html", error=error)

    except Exception as e:
        #flash(e)
        error = "Invalid credentials, try again."
        return render_template("login.html", error = error)
