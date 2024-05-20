import pygame
import sys
import math

# Размеры окна
WIDTH, HEIGHT = 800, 600

# Цвета
WHITE = (255, 255, 255)

# Координаты вершин куба
cube_template = [
    [-1, -1, -1], [1, -1, -1], [1, 1, -1], [-1, 1, -1],
    [-1, -1, 1], [1, -1, 1], [1, 1, 1], [-1, 1, 1]
]

# Грани куба
faces = [[0, 4, 5, 1], [0, 1, 2, 3], [0, 3, 7, 4], [5, 4, 7, 6], [1, 5, 6, 2], [2, 6, 7, 3]]
# Цвета для граней куба
colors = [
    (0, 100, 255),
    (100, 255, 0),
    (255, 100, 0),
    (0, 255, 255),
    (255, 0, 255),
    (255, 255, 0)
]

# Массив для спроецированных точек
points = []

# Угол вращения
angle = 0

# Направление света (пример освещения сверху)
light_direction = [1, 1, 1]

# Функция для поворота точки вокруг оси OY
def rotate_y(point, angle):
    x, y, z = point
    new_x = x * math.cos(angle) - z * math.sin(angle)
    new_z = x * math.sin(angle) + z * math.cos(angle)
    return [new_x, y, new_z]

# Функция для одноточечной центральной проекции
def project(point):
    fov = 256  # Угол обзора (Field of View)
    viewer_distance = 4  # Расстояние от камеры до проекционной плоскости
    x, y, z = point
    new_x = x * fov / (z + viewer_distance) + WIDTH / 2
    new_y = y * fov / (z + viewer_distance) + HEIGHT / 2
    return [int(new_x), int(new_y)]

# Проверка условия видимости грани
# Функция для вычисления нормали к грани
def calculate_normal(face):
    # Вычисляем векторы между точками грани
    vector1 = [cube_template[face[1]][0] - cube_template[face[0]][0],
               cube_template[face[1]][1] - cube_template[face[0]][1],
               cube_template[face[1]][2] - cube_template[face[0]][2]]
    vector2 = [cube_template[face[2]][0] - cube_template[face[0]][0],
               cube_template[face[2]][1] - cube_template[face[0]][1],
               cube_template[face[2]][2] - cube_template[face[0]][2]]
    # Вычисляем векторное произведение
    normal = [
        vector1[1] * vector2[2] - vector1[2] * vector2[1],
        vector1[2] * vector2[0] - vector1[0] * vector2[2],
        vector1[0] * vector2[1] - vector1[1] * vector2[0]
    ]
    return normal

# Проверка направления нормали к грани относительно направления камеры
def is_backface(face):
    normal = calculate_normal(face)
    camera_direction = [0, 0, -1]  # направление камеры
    dot_product = sum([a * b for a, b in zip(normal, camera_direction)])
    return dot_product < 0

# Инициализация Pygame
pygame.init()
screen = pygame.display.set_mode((WIDTH, HEIGHT))
clock = pygame.time.Clock()

# Загрузка изображения для заднего плана
background_image_path = 'sem-6/task/bitmap.bmp'  # путь к загруженному изображению
try:
    background = pygame.image.load(background_image_path).convert()
    background = pygame.transform.scale(background, (WIDTH, HEIGHT))
except pygame.error as e:
    print("Error loading background image:", e)
    background = None

def is_visible(face):
    x0, y0 = points[face[0]]
    x1, y1 = points[face[1]]
    x2, y2 = points[face[2]]
    x3, y3 = points[face[3]]
    S = (x0 - x1) * (y0 + y1) + (x1 - x2) * (y1 + y2) + (x2 - x3) * (y2 + y3) + (x3 - x0) * (y3 + y0)
    return S > 0

# Основной цикл
running = True
while running:
    # Обработка событий
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

    # Обработка нажатия клавиш
    keys = pygame.key.get_pressed()
    if keys[pygame.K_LEFT]:
        angle -= 0.05
    if keys[pygame.K_RIGHT]:
        angle += 0.05

    # Очистка массива точек
    points.clear()

    # Вращение и проецирование точек куба
    for point in cube_template:
        rotated_point = rotate_y(point, angle)
        projected_point = project(rotated_point)
        points.append(projected_point)

    # Отрисовка заднего плана
    screen.fill(WHITE)
    if background:
        screen.blit(background, (0, 0))

    # Отрисовка граней куба
    for i, face in enumerate(faces):
        if is_visible(face):
            pygame.draw.polygon(screen, colors[i], [points[vertex][:2] for vertex in face])

    pygame.display.flip()
    clock.tick(60)

pygame.quit()
sys.exit()
