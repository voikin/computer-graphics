import pygame
import sys
import math

# Инициализация Pygame
pygame.init()

# Установка параметров окна
width, height = 800, 600
window = pygame.display.set_mode((width, height))
pygame.display.set_caption("Вращающийся четырёхугольник")

# Определение цветов
BLACK = (0, 0, 0)
WHITE = (255, 255, 255)

# Задание вершин четырёхугольника
size = 250
half_size = size / 2
quad = [
    (-half_size, -half_size),
    (half_size, -half_size),
    (half_size, half_size),
    (-half_size, half_size)
]

# Функция для вращения точек вокруг центра
def rotate_points(points, angle, center):
    rotated_points = []
    for point in points:
        x, y = point
        cos_theta = math.cos(angle)
        sin_theta = math.sin(angle)
        new_x = x * cos_theta - y * sin_theta
        new_y = x * sin_theta + y * cos_theta
        rotated_points.append((new_x + center[0], new_y + center[1]))
    return rotated_points

# Настройки таймера
clock = pygame.time.Clock()
rotation_speed = 0.01  # Радианы за кадр

# Основной цикл
running = True
angle = 0
while running:
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
    
    # Обновление угла
    angle -= rotation_speed
    
    # Очистка экрана
    window.fill(BLACK)
    
    # Вычисление повернутых вершин
    center = (width // 2, height // 2)
    rotated_quad = rotate_points(quad, angle, center)
    
    # Рисование четырёхугольника
    pygame.draw.polygon(window, WHITE, rotated_quad, 1)
    
    # Двойная буферизация
    pygame.display.flip()
    
    # Частота кадров
    clock.tick(60)

# Завершение Pygame
pygame.quit()
sys.exit()
