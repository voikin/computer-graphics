using System;
using System.Windows.Forms;
using OpenTK;
using OpenTK.Graphics.OpenGL;

namespace personal_task_222
{
    public partial class Form1 : Form
    {
        // Объявляем элемент управления GLControl для работы с OpenGL
        private GLControl glControl;
        // Матрица преобразований, инициализированная как единичная матрица
        private Matrix4 transformMatrix = Matrix4.Identity;
        // Панель управления для размещения кнопок
        private FlowLayoutPanel controlPanel;

        // Конструктор формы
        public Form1()
        {
            InitializeComponent();
            SetupGLControl();
            SetupInterface();
        }

        // Настройка элемента управления GLControl
        private void SetupGLControl()
        {
            glControl = new GLControl();
            glControl.Dock = DockStyle.Fill;
            this.Controls.Add(glControl);
            glControl.Paint += GlControl_Paint;
            glControl.Resize += GlControl_Resize;
            glControl.Load += GlControl_Load;
        }

        // Обработчик события загрузки GLControl
        private void GlControl_Load(object sender, EventArgs e)
        {
            // Устанавливаем цвет фона
            GL.ClearColor(System.Drawing.Color.White);
            // Включаем тест глубины
            GL.Enable(EnableCap.DepthTest);

            // Настройка освещения
            GL.Enable(EnableCap.Lighting);
            GL.Enable(EnableCap.Light0);
            GL.Light(LightName.Light0, LightParameter.Position, new float[] { 0.0f, 0.0f, 1.0f, 0.0f });
            GL.Light(LightName.Light0, LightParameter.Diffuse, new float[] { 1.0f, 1.0f, 1.0f, 1.0f });

            // Настройка материала
            GL.Material(MaterialFace.Front, MaterialParameter.Diffuse, new float[] { 1.0f, 0.5f, 0.5f, 1.0f });
            GL.ShadeModel(ShadingModel.Smooth);
        }

        // Обработчик события перерисовки GLControl
        private void GlControl_Paint(object sender, PaintEventArgs e)
        {
            Render();
        }

        // Обработчик события изменения размеров GLControl
        private void GlControl_Resize(object sender, EventArgs e)
        {
            GL.Viewport(0, 0, glControl.Width, glControl.Height);
            GL.MatrixMode(MatrixMode.Projection);
            GL.LoadIdentity();
            GL.Ortho(-1, 1, -1, 1, -1, 1);
        }

        // Метод для отрисовки сцены
        private void Render()
        {
            GL.Clear(ClearBufferMask.ColorBufferBit | ClearBufferMask.DepthBufferBit);
            GL.MatrixMode(MatrixMode.Modelview);
            GL.LoadIdentity();
            GL.MultMatrix(ref transformMatrix);

            DrawShape();

            glControl.SwapBuffers();
        }

        // Метод для отрисовки фигуры
        private void DrawShape()
        {
            GL.Begin(PrimitiveType.Quads);

            // Нижняя грань
            GL.Normal3(0.0f, 0.0f, -1.0f);
            GL.Color3(1.0f, 0.0f, 0.0f); // Красный
            GL.Vertex3(-0.5f, -0.5f, 0.0f);
            GL.Vertex3(0.5f, -0.5f, 0.0f);
            GL.Vertex3(0.5f, 0.5f, 0.0f);
            GL.Vertex3(-0.5f, 0.5f, 0.0f);

            // Верхняя грань
            GL.Normal3(0.0f, 0.0f, 1.0f);
            GL.Color3(0.0f, 1.0f, 0.0f); // Зеленый
            GL.Vertex3(-0.5f, -0.5f, 0.5f);
            GL.Vertex3(0.5f, -0.5f, 0.5f);
            GL.Vertex3(0.3f, 0.5f, 0.5f);
            GL.Vertex3(-0.3f, 0.5f, 0.5f);

            // Передняя грань
            GL.Normal3(0.0f, 1.0f, 0.0f);
            GL.Color3(0.0f, 0.0f, 1.0f); // Синий
            GL.Vertex3(-0.5f, 0.5f, 0.0f);
            GL.Vertex3(0.5f, 0.5f, 0.0f);
            GL.Vertex3(0.3f, 0.5f, 0.5f);
            GL.Vertex3(-0.3f, 0.5f, 0.5f);

            // Задняя грань
            GL.Normal3(0.0f, -1.0f, 0.0f);
            GL.Color3(1.0f, 1.0f, 0.0f); // Желтый
            GL.Vertex3(-0.5f, -0.5f, 0.0f);
            GL.Vertex3(0.5f, -0.5f, 0.0f);
            GL.Vertex3(0.5f, -0.5f, 0.5f);
            GL.Vertex3(-0.5f, -0.5f, 0.5f);

            // Левая грань
            GL.Normal3(-1.0f, 0.0f, 0.0f);
            GL.Color3(1.0f, 0.0f, 1.0f); // Магента
            GL.Vertex3(-0.5f, -0.5f, 0.0f);
            GL.Vertex3(-0.5f, 0.5f, 0.0f);
            GL.Vertex3(-0.3f, 0.5f, 0.5f);
            GL.Vertex3(-0.5f, -0.5f, 0.5f);

            // Правая грань
            GL.Normal3(1.0f, 0.0f, 0.0f);
            GL.Color3(0.0f, 1.0f, 1.0f); // Голубой
            GL.Vertex3(0.5f, -0.5f, 0.0f);
            GL.Vertex3(0.5f, 0.5f, 0.0f);
            GL.Vertex3(0.3f, 0.5f, 0.5f);
            GL.Vertex3(0.5f, -0.5f, 0.5f);

            GL.End();
        }

        // Метод для перемещения объекта
        private void Translate(float dx, float dy)
        {
            transformMatrix *= Matrix4.CreateTranslation(dx, dy, 0);
            glControl.Invalidate();
        }

        // Метод для масштабирования объекта
        private void Scale(float sx, float sy)
        {
            transformMatrix *= Matrix4.CreateScale(sx, sy, 1.0f);
            glControl.Invalidate();
        }

        // Метод для вращения объекта
        private void Rotate(float angle, Vector3 axis)
        {
            transformMatrix *= Matrix4.CreateFromAxisAngle(axis, MathHelper.DegreesToRadians(angle));
            glControl.Invalidate();
        }

        // Метод для отражения объекта относительно осей
        private void Reflect(bool xAxis, bool yAxis)
        {
            Matrix4 reflectMatrix = Matrix4.Identity;
            if (xAxis) reflectMatrix.M11 = -1;
            if (yAxis) reflectMatrix.M22 = -1;
            transformMatrix *= reflectMatrix;
            glControl.Invalidate();
        }

        // Метод для отражения объекта относительно прямой y=x
        private void ReflectYEqualsX()
        {
            transformMatrix *= new Matrix4(
                0, 1, 0, 0,
                1, 0, 0, 0,
                0, 0, 1, 0,
                0, 0, 0, 1
            );
            glControl.Invalidate();
        }

        // Метод для сброса всех преобразований
        private void ResetTransformations()
        {
            transformMatrix = Matrix4.Identity;
            glControl.Invalidate();
        }

        // Настройка пользовательского интерфейса с кнопками управления
        private void SetupInterface()
        {
            controlPanel = new FlowLayoutPanel();
            controlPanel.Dock = DockStyle.Right;
            controlPanel.Width = 150;
            controlPanel.FlowDirection = FlowDirection.TopDown;
            controlPanel.AutoScroll = true;
            this.Controls.Add(controlPanel);

            // Добавление кнопок с их обработчиками событий
            AddButton("Translate", (s, e) => Translate(0.1f, 0.1f));
            AddButton("Scale", (s, e) => Scale(1.1f, 1.1f));
            AddButton("Rotate X", (s, e) => Rotate(10.0f, Vector3.UnitX));
            AddButton("Rotate Y", (s, e) => Rotate(10.0f, Vector3.UnitY));
            AddButton("Rotate Z", (s, e) => Rotate(10.0f, Vector3.UnitZ));
            AddButton("Reflect X", (s, e) => Reflect(true, false));
            AddButton("Reflect Y", (s, e) => Reflect(false, true));
            AddButton("Reflect Y=X", (s, e) => ReflectYEqualsX());
            AddButton("Reset", (s, e) => ResetTransformations());
        }

        // Метод для добавления кнопок на панель управления
        private void AddButton(string text, EventHandler onClick)
        {
            var button = new Button { Text = text, AutoSize = true, Margin = new Padding(5) };
            button.Click += onClick;
            controlPanel.Controls.Add(button);
        }
    }
}
