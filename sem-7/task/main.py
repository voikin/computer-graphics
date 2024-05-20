import pygame
import sys
import math

# Размеры окна
SCREEN_WIDTH, SCREEN_HEIGHT = 800, 600

# Цвета
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# Координаты вершин куба
cube_vertices = [
    [-1, -1, -1], [1, -1, -1], [1, 1, -1], [-1, 1, -1],
    [-1, -1, 1], [1, -1, 1], [1, 1, 1], [-1, 1, 1]
]

# Грани куба
cube_faces = [
    [0, 1, 2, 3],  # Нижняя грань
    [7, 6, 5, 4],  # Верхняя грань
    [3, 7, 4, 0],  # Левая грань
    [1, 5, 6, 2],  # Правая грань
    [0, 4, 5, 1],  # Передняя грань
    [2, 6, 7, 3]   # Задняя грань
]

# Цвета для граней куба
face_colors = [
    (0, 100, 255),
    (100, 255, 0),
    (255, 100, 0),
    (0, 255, 255),
    (255, 0, 255),
    (255, 255, 0)
]

# Инициализация Pygame
pygame.init()
window = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
fps_clock = pygame.time.Clock()

# Загрузка и масштабирование фонового изображения
background_img = pygame.image.load('sem-7/task/bitmap.bmp')
background_img = pygame.transform.scale(background_img, (SCREEN_WIDTH, SCREEN_HEIGHT))

# Углы вращения
rotation_x = 0
rotation_y = 0

def rotate_along_x(point, angle):
    """
    Вращение точки вокруг оси X.
    """
    x, y, z = point
    rotated_y = y * math.cos(angle) - z * math.sin(angle)
    rotated_z = y * math.sin(angle) + z * math.cos(angle)
    return [x, rotated_y, rotated_z]

def rotate_along_y(point, angle):
    """
    Вращение точки вокруг оси Y.
    """
    x, y, z = point
    rotated_x = x * math.cos(angle) - z * math.sin(angle)
    rotated_z = x * math.sin(angle) + z * math.cos(angle)
    return [rotated_x, y, rotated_z]

def project_to_2d(point):
    """
    Проецирование 3D-точки в 2D-пространство.
    """
    fov = 256  # Поле зрения
    viewer_dist = 4  # Расстояние до зрителя
    x, y, z = point
    scale = fov / (z + viewer_dist)
    proj_x = x * scale + SCREEN_WIDTH / 2
    proj_y = y * scale + SCREEN_HEIGHT / 2
    return [int(proj_x), int(proj_y)]

def face_is_visible(face, points):
    """
    Проверка видимости грани.
    """
    x0, y0 = points[face[0]][:2]
    x1, y1 = points[face[1]][:2]
    x2, y2 = points[face[2]][:2]
    x3, y3 = points[face[3]][:2]
    area = (x0 - x1) * (y0 + y1) + (x1 - x2) * (y1 + y2) + (x2 - x3) * (y2 + y3) + (x3 - x0) * (y3 + y0)
    return area > 0

def compute_shadow_intensity(face, points):
    """
    Вычисление интенсивности тени для грани.
    """
    light_dir = (1, 1, 0)  # Направление источника света
    x0, y0, z0 = points[face[0]]
    x1, y1, z1 = points[face[1]]
    x2, y2, z2 = points[face[2]]
    # Вычисляем нормаль к грани
    normal = (
        (y1 - y0) * (z2 - z0) - (z1 - z0) * (y2 - y0),
        (z1 - z0) * (x2 - x0) - (x1 - x0) * (z2 - z0),
        (x1 - x0) * (y2 - y0) - (y1 - y0) * (x2 - x0)
    )
    normal_length = math.sqrt(normal[0] ** 2 + normal[1] ** 2 + normal[2] ** 2)
    light_length = math.sqrt(light_dir[0] ** 2 + light_dir[1] ** 2 + light_dir[2] ** 2)
    cos_theta = (normal[0] * light_dir[0] + normal[1] * light_dir[1] + normal[2] * light_dir[2]) / (normal_length * light_length)
    shadow = max(0, cos_theta)
    return shadow

# Основной цикл
is_running = True
while is_running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            pygame.quit()
            sys.exit()

    # Обработка нажатий клавиш для вращения куба
    key_input = pygame.key.get_pressed()
    rotation_x += (key_input[pygame.K_UP] - key_input[pygame.K_DOWN]) * 0.05
    rotation_y += (key_input[pygame.K_RIGHT] - key_input[pygame.K_LEFT]) * 0.05

    # Вращение и проецирование точек куба
    transformed_points = [rotate_along_y(rotate_along_x(p, rotation_x), rotation_y) for p in cube_vertices]
    projected_2d_points = [project_to_2d(p) for p in transformed_points]

    # Отображение фонового изображения
    window.blit(background_img, (0, 0))

    # Отрисовка граней куба
    for i, face in enumerate(cube_faces):
        if face_is_visible(face, projected_2d_points):
            shadow = compute_shadow_intensity(face, transformed_points)
            face_color = tuple(int(val * (1 - shadow)) for val in face_colors[i])
            try:
                pygame.draw.polygon(window, face_color, [projected_2d_points[vertex] for vertex in face])
            except ValueError:
                print("Недопустимое значение цвета:", face_color)

    # Обновление экрана
    pygame.display.flip()
    # Ограничение частоты кадров
    fps_clock.tick(60)

   

