#include <QApplication>
#include <QWidget>
#include <QTimer>
#include <QElapsedTimer>
#include <QPainter>
#include <QObject>
#include <QMouseEvent>
#include <QRandomGenerator>

class Window : public QWidget
{
    Q_OBJECT
public:
    Window(int w, int h) : WIDTH(w), HEIGHT(h)
    {
        setWindowTitle(QStringLiteral("Falling Sand Qt/C++"));
        resize(w, h);
        m_cols = WIDTH / m_size;
        m_rows = HEIGHT / m_size;
        make2DArray(m_grid);


        QObject::connect(&m_timer, &QTimer::timeout, this, &Window::animationLoop);
        m_timer.start(1000.f/60.f); //60 times per second
        m_elapsedTimer.start();
    }
    virtual ~Window(){}

private:
    QTimer m_timer;
    QElapsedTimer m_elapsedTimer;
    const int WIDTH;
    const int HEIGHT;
    std::array< std::array<int, 100>, 100> m_grid;
    std::array< std::array<int, 100>, 100> m_nextGrid;
    int m_cols, m_rows;
    const int m_size = 5;
    bool m_clicked = false;
    int m_hueValue = 200;
    void make2DArray(std::array< std::array<int, 100>, 100>& grid) {
        for (int x = 0; x < m_cols; ++x) {
            for (int y = 0; y < m_rows; ++y) {
                grid[x][y] = 0;
            }
        }
    }
    // Check if a row is within the bounds
    int withinCols(int i) {
        return i >= 0 && i <= this->m_cols - 1;
    }

    // Check if a column is within the bounds
    int withinRows(int j) {
        return j >= 0 && j <= this->m_rows - 1;
    }
private slots:
    void animationLoop()
    {
        update();
    }
private:
    void paintEvent(QPaintEvent *event)
    {
        Q_UNUSED(event);
        QPainter qp(this);
        qp.setBrush(QBrush(Qt::black));
        qp.drawRect(0,0,size().width(), size().height());

        //Draw the sand
        qp.setPen(QPen(Qt::NoPen));
        for (int i = 0; i < m_cols; ++i) {
            for (int j = 0; j < m_rows; ++j) {
                if (m_grid[i][j] > 0) {
                    QColor color = QColor::fromHsl(m_grid[i][j], 210, 210);
                    qp.setBrush(QBrush(color));
                    int x = i * m_size;
                    int y = j * m_size;
                    qp.drawRect(x, y, m_size, m_size);
                }
            }
        }

        // Create a 2D array for the next frame of animation
        make2DArray(m_nextGrid);

        // Check every cell
        for (int i = 0; i < m_cols; i++) {
            for (int j = 0; j < m_rows; j++) {
                // What is the state?
                const int state = m_grid[i][j];

                // If it's a piece of sand!
                if (state > 0) {
                    // What is below?
                    const int below = m_grid[i][j + 1];

                    // Randomly fall left or right
                    int dir = 1;
                    if (QRandomGenerator::global()->generateDouble() < 0.5) {
                        dir *= -1;
                    }

                    // Check below left or right
                    int belowA = -1;
                    int belowB = -1;
                    if (withinCols(i + dir)) {
                        belowA = m_grid[i + dir][j + 1];
                    }
                    if (withinCols(i - dir)) {
                        belowB = m_grid[i - dir][j + 1];
                    }


                    // Can it fall below or left or right?
                    if (below == 0) {
                        m_nextGrid[i][j + 1] = state;
                    } else if (belowA == 0) {
                        m_nextGrid[i + dir][j + 1] = state;
                    } else if (belowB == 0) {
                        m_nextGrid[i - dir][j + 1] = state;
                        // Stay put!
                    } else {
                        m_nextGrid[i][j] = state;
                    }
                }
            }
        }

        m_grid = m_nextGrid;
    }

    // QWidget interface
protected:
    void mousePressEvent(QMouseEvent *) {
        m_clicked = true;
    }

    void mouseReleaseEvent(QMouseEvent *) {
        m_clicked = false;
    }

    void mouseMoveEvent(QMouseEvent *event) {
        if (m_clicked) {
            int mouseX = event->pos().x();
            int mouseY = event->pos().y();
            int mouseCol = int(mouseX / m_size);
            int mouseRow = int(mouseY / m_size);
            const int matrix = 5;
            const int extent = int(matrix / 2);
            for (int i = -extent; i <= extent; i++) {
                for (int j = -extent; j <= extent; j++) {
                    float randomValue = QRandomGenerator::global()->generateDouble();
                    //qDebug() << "rV " << randomValue;
                    if ( randomValue < 0.75) {
                        int col = mouseCol + i;
                        int row = mouseRow + j;
                        if (withinCols(col) && withinRows(row)) {
                            m_grid[col][row] = m_hueValue;
                        }
                    }
                }
            }

            // Change the color of the sand over time
            m_hueValue += 2;
            if (m_hueValue > 300) {
                m_hueValue = 1;
            }
        }
    }
};

#include "main.moc"

int main(int argc, char** argv)
{
    QApplication a(argc, argv);
    Window *w = new Window(500, 300);
    w->show();
    a.exec();
}


