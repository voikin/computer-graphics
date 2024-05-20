import math
import turtle

# Функция для преобразования координат (x, y) в координаты на экране turtle
def convert_coordinates(x, y, screen_width, screen_height, scale_x, scale_y, shift_x, shift_y):
    screen_x = x * scale_x + shift_x
    screen_y = y * scale_y + shift_y
    return screen_x, screen_y

# Установка экрана turtle
screen = turtle.Screen()
screen.setup(width=800, height=600)
screen.setworldcoordinates(-4 * math.pi, -2, 4 * math.pi, 2)

# Инициализация черепашки
t = turtle.Turtle()
t.speed(0)
t.penup()

# Параметры сдвига и масштабирования
shift_x = math.pi / 2  # Сдвиг в направлении x
shift_y = 1  # Сдвиг в направлении y
scale_x = 0.5  # Масштабирование в направлении x
scale_y = 2  # Масштабирование в направлении y

# Отрисовка осей координат
t.goto(-4 * math.pi, 0)
t.pendown()
t.goto(4 * math.pi, 0)
t.penup()
t.goto(0, -2)
t.pendown()
t.goto(0, 2)
t.penup()

# Отрисовка графика функции y = sin(x)
x = -4 * math.pi
while x <= 4 * math.pi:
    y = math.sin(x)
    screen_x, screen_y = convert_coordinates(x, y, 800, 600, scale_x, scale_y, shift_x, shift_y)
    t.goto(screen_x, screen_y)
    t.pendown()
    x += 0.1
t.penup()

# Скрыть черепашку и показать результат
t.hideturtle()
turtle.done()
