import math
import random
import matplotlib.pyplot as plt

# Генерация случайных точек
def generate_random_points(num_points, min_val, max_val):
    points = [(random.uniform(min_val, max_val), random.uniform(min_val, max_val)) for _ in range(num_points)]
    return points

# Вспомогательная функция для вычисления полярного угла
def polar_angle(p0, p1):
    y_span = p1[1] - p0[1]
    x_span = p1[0] - p0[0]
    return math.atan2(y_span, x_span)

# Вспомогательная функция для вычисления площади треугольника
def area(p0, p1, p2):
    return (p1[0] - p0[0]) * (p2[1] - p0[1]) - (p1[1] - p0[1]) * (p2[0] - p0[0])

# Алгоритм Грэхема для построения выпуклой оболочки
def graham_scan(points):
    # Шаг 1: Найти начальную точку
    start = min(points, key=lambda p: (p[1], p[0]))
    points.pop(points.index(start))
    
    # Шаг 2: Сортировать точки по полярному углу относительно начальной точки
    points.sort(key=lambda p: (polar_angle(start, p), -p[1], p[0]))
    
    # Шаг 3: Инициализация стека и добавление начальной точки
    hull = [start]
    
    for point in points:
        while len(hull) > 1 and area(hull[-2], hull[-1], point) <= 0:
            hull.pop()
        hull.append(point)
    
    return hull

# Функция для визуализации выпуклой оболочки
def plot_convex_hull(points, hull):
    plt.figure()
    plt.plot([p[0] for p in points], [p[1] for p in points], 'ro')
    hull.append(hull[0])
    plt.plot([p[0] for p in hull], [p[1] for p in hull], 'b-')
    plt.show()

# Генерация точек и построение выпуклой оболочки
num_points = 100
min_val = 0
max_val = 100
points = generate_random_points(num_points, min_val, max_val)
hull = graham_scan(points)

# Визуализация результата
plot_convex_hull(points, hull)
