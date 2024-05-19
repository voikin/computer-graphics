import pygame
import numpy as np
import math

# Инициализация Pygame
pygame.init()

# Задаем размеры окна
width, height = 800, 600
screen = pygame.display.set_mode((width, height))
pygame.display.set_caption("Bresenham Drawing and Transformations")

# Задаем цвета
black = (0, 0, 0)
white = (255, 255, 255)

# Алгоритм Брезенхэма для рисования прямой
def draw_line(x0, y0, x1, y1):
    points = []
    dx = abs(x1 - x0)
    dy = abs(y1 - y0)
    sx = 1 if x0 < x1 else -1
    sy = 1 if y0 < y1 else -1
    err = dx - dy

    while True:
        points.append((x0, y0))
        if x0 == x1 and y0 == y1:
            break
        e2 = err * 2
        if e2 > -dy:
            err -= dy
            x0 += sx
        if e2 < dx:
            err += dx
            y0 += sy
    return points

# Алгоритм Брезенхэма для рисования окружности
def draw_circle(xc, yc, r):
    points = []
    x = 0
    y = r
    d = 3 - 2 * r
    while y >= x:
        points.extend([(xc + x, yc + y), (xc - x, yc + y), (xc + x, yc - y), (xc - x, yc - y),
                       (xc + y, yc + x), (xc - y, yc + x), (xc + y, yc - x), (xc - y, yc - x)])
        x += 1
        if d > 0:
            y -= 1
            d = d + 4 * (x - y) + 10
        else:
            d = d + 4 * x + 6
    return points

# Аффинное преобразование точки
def transform_point(x, y, matrix):
    point = np.array([x, y, 1])
    transformed_point = matrix @ point
    return int(transformed_point[0]), int(transformed_point[1])

# Отрисовка фигуры с применением аффинных преобразований
def draw_figure(buffer, x, y, scale, angle, pivot):
    # Поворотная матрица
    rad = math.radians(angle)
    cos_theta = math.cos(rad)
    sin_theta = math.sin(rad)
    translate_to_origin = np.array([[1, 0, -pivot[0]], [0, 1, -pivot[1]], [0, 0, 1]])
    rotate = np.array([[cos_theta, -sin_theta, 0], [sin_theta, cos_theta, 0], [0, 0, 1]])
    translate_back = np.array([[1, 0, pivot[0]], [0, 1, pivot[1]], [0, 0, 1]])
    rotation_matrix = translate_back @ rotate @ translate_to_origin

    # Масштабирующая матрица
    scale_matrix = np.array([[scale, 0, x * (1 - scale)], [0, scale, y * (1 - scale)], [0, 0, 1]])

    # Комбинированная матрица трансформации
    transform_matrix = scale_matrix @ rotation_matrix

    circle_points = draw_circle(x, y, int(50))
    square1_points = draw_line(x - 50, y + 50, x - 50, y + 100)
    square1_points += draw_line(x - 50, y + 100, x - 25, y + 100)
    square1_points += draw_line(x - 25, y + 100, x - 25, y + 50)
    square1_points += draw_line(x - 25, y + 50, x - 50, y + 50)
    
    square2_points = draw_line(x + 50, y + 50, x + 50, y + 100)
    square2_points += draw_line(x + 50, y + 100, x + 25, y + 100)
    square2_points += draw_line(x + 25, y + 100, x + 25, y + 50)
    square2_points += draw_line(x + 25, y + 50, x + 50, y + 50)

    all_points = circle_points + square1_points + square2_points

    for point in all_points:
        transformed_point = transform_point(point[0], point[1], transform_matrix)
        buffer.set_at(transformed_point, black)

# Основной цикл программы
running = True
mode = 'rotate'  # initial mode
scale = 1.0
angle = 0
x, y = 200, 200
pivot = (50, 100)

# Создание буфера для рисования
buffer = pygame.Surface((width, height))

while running:
    buffer.fill(white)
    
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:  # ЛКМ
                if mode == 'rotate':
                    angle += 10
                elif mode == 'scale':
                    scale += 0.1
            elif event.button == 3:  # ПКМ
                mode = 'scale' if mode == 'rotate' else 'rotate'
    
    draw_figure(buffer, x, y, scale, angle, pivot)
    screen.blit(buffer, (0, 0))
    pygame.display.flip()
    pygame.time.delay(100)

pygame.quit()
