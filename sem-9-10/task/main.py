import pygame
import sys
import math

# Размеры окна
SCREEN_WIDTH, SCREEN_HEIGHT = 800, 600

# Цвета
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)

# Яркость источника и рассеянный свет
amp = 0.9
ambient = 0.4
light = [-0.5, -0.5, -0.5]

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

# Углы вращения
rotation_x = 0
rotation_y = 0

def rotate_along_x(point, angle):
    """ Вращение точки вокруг оси X. """
    x, y, z = point
    rotated_y = y * math.cos(angle) - z * math.sin(angle)
    rotated_z = y * math.sin(angle) + z * math.cos(angle)
    return [x, rotated_y, rotated_z]

def rotate_along_y(point, angle):
    """ Вращение точки вокруг оси Y. """
    x, y, z = point
    rotated_x = x * math.cos(angle) - z * math.sin(angle)
    rotated_z = x * math.sin(angle) + z * math.cos(angle)
    return [rotated_x, y, rotated_z]

def project_to_2d(point):
    """ Проецирование 3D-точки в 2D-пространство. """
    fov = 256  # Поле зрения
    viewer_dist = 4  # Расстояние до зрителя
    x, y, z = point
    scale = fov / (z + viewer_dist)
    proj_x = x * scale + SCREEN_WIDTH / 2
    proj_y = y * scale + SCREEN_HEIGHT / 2
    return [int(proj_x), int(proj_y)]

def normalize(vector):
    """ Нормализация вектора. """
    length = math.sqrt(sum(coord ** 2 for coord in vector))
    return [coord / length for coord in vector]

def compute_vertex_intensity(vertex):
    """ Вычисление интенсивности света в вершине. """
    light_dir = normalize(light)
    normal = normalize(vertex)
    dot_product = sum(normal[i] * light_dir[i] for i in range(3))
    intensity = ambient + amp * max(0, dot_product)
    return min(1, intensity)

def interpolate_color(color, intensity):
    """ Интерполяция цвета с учетом интенсивности. """
    r, g, b = color
    return (int(r * intensity), int(g * intensity), int(b * intensity))

def gouraud_shading(face, vertices, face_color):
    """ Закраска грани методом Гуро. """
    intensities = [compute_vertex_intensity(vertices[vertex]) for vertex in face]
    avg_intensity = sum(intensities) / len(intensities)
    shaded_color = interpolate_color(face_color, avg_intensity)
    
    pygame.draw.polygon(window, shaded_color, [projected_2d_points[vertex] for vertex in face])

def is_visible(face, vertices):
    """ Проверка видимости грани. """
    projected_points = [projected_2d_points[vertex] for vertex in face]
    x0, y0 = projected_points[0]
    x1, y1 = projected_points[1]
    x2, y2 = projected_points[2]
    x3, y3 = projected_points[3]
    
    S = (x0 - x1) * (y0 + y1) + (x1 - x2) * (y1 + y2) + (x2 - x3) * (y2 + y3) + (x3 - x0) * (y3 + y0)
    return S > 0

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
    window.fill(BLACK)

    # Отрисовка граней куба
    for i, face in enumerate(cube_faces):
        if is_visible(face, transformed_points):
            gouraud_shading(face, transformed_points, face_colors[i])

    # Обновление экрана
    pygame.display.flip()
    # Ограничение частоты кадров
    fps_clock.tick(60)
