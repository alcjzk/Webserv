import cmath
import os
import cgi
import time
import string
import random

form = cgi.FieldStorage()

def brot(c):
    itmax = 255
    z = complex(0, 0)
    while (z.real < 5.0 and z.imag < 5.0 and itmax > 0):
        z = z**2 + c
        itmax -= 1
    return itmax;

if __name__ == "__main__":

    qs = os.environ['QUERY_STRING']

    print("""Content-type: text/html

          <!DOCTYPE HTML>
            <html>
              <head>
            p {
                font-family: numberFont, regularFont
            }
            </head>
            <body>
            <pre>
          """)
    print(f'QUERY_STRING: {qs}')

    d = {}
    vals = qs.split('&')
    print(f'vals: {vals}')
    for val in vals:
        if '=' not in val:
            continue
        key_val = val.split('=')
        d[key_val[0]] = float(key_val[1])
    print(f'd: {d}')
    print('form', form)
    if form.getvalue('x_left') is not None:
        x_left = float(form.getvalue('x_left'))
    else:
        x_left = -1.5

    if form.getvalue('x_right') is not None:
        x_right = float(form.getvalue('x_right'))
    else:
        x_right = 0.5

    if form.getvalue('y_bottom') is not None:
        y_bottom = float(form.getvalue('y_bottom'))
    else:
        y_bottom = -1.0

    if form.getvalue('y_top') is not None:
        y_top = float(form.getvalue('y_top'))
    else:
        y_top = 1.0

    if form.getvalue('x_res') is not None:
        x_res = float(form.getvalue('x_res'))
    else:
        x_res = 50.0
    if form.getvalue('y_res') is not None:
        y_res = float(form.getvalue('y_res'))
    else:
        y_res = 50.0

    start = complex(x_left, y_top)
    end = complex(x_right, y_bottom)

    x_step = abs(x_left - x_right) / x_res
    y_step = abs(y_bottom - y_top) / y_res
    print(f'x_left: {x_left}     x_right: {x_right}')
    print(f'y_bottom: {y_bottom} y_top: {y_top}')
    print(f'x_step: {x_step}     y_step: {y_step}')
    print(f'x_res: {x_res}       y_res: {y_res}')

    print("</pre>")
    y_inc = y_top
    while y_inc > y_bottom:
        x_inc = x_left
        while x_inc < x_right:
            color = brot(complex(x_inc, y_inc))
            if color:
                print(f"<p style=\"display:inline;color:rgb({color % 255}, {color * 2 % 255}, {color * 4 % 255})\">{random.choice(string.ascii_letters)}</p>", end="")
            else:
                print(f"<p style=\"display:inline;color:rgb({color % 255}, {color * 2 % 255}, {color * 4 % 255})\">{random.choice(string.ascii_letters)}</p>", end="")
            x_inc += x_step
        print("<br>")
        y_inc -= y_step
    print(f"""
        <form action="/mandelbrot/mandelbrot.py" method="POST">

            <label for="x_left">x_left:</label><br>
            <input type="number" id="x_left" name="x_left" value="{x_left}"><br>
            <label for="x_right">x_right:</label><br>
            <input type="number" id="x_right" name="x_right" value="{x_right}"><br><br>

            <label for="y_top">y_top:</label><br>
            <input type="number" id="y_top" name="y_top" value="{y_top}"><br><br>
            <label for="y_bottom">y_bottom:</label><br>
            <input type="number" id="y_bottom" name="y_bottom" value="{y_bottom}"><br><br>

            <label for="x_res">x_res:</label><br>
            <input type="number" id="x_res" name="x_res" value="{x_res}"><br>
            <label for="y_res">y_res:</label><br>
            <input type="number" id="y_res" name="y_res" value="{y_res}"><br><br>

            <input type="submit" value="Submit">
        </form>

          """)
    print("""
        </body>
        </html>
          """)
