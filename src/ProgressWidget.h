/**
 * @file ProgressWidget.h
 * @brief Show the progress
 */
#ifndef PROGRESSWIDGET_H
#define PROGRESSWIDGET_H


#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QVBoxLayout>

/**
 * @brief 顯示進度的視窗
 */
class ProgressWidget : public QWidget
{
    Q_OBJECT

    QVBoxLayout* m_layout;
    QLabel* m_text;
    QProgressBar* m_progress;

public:
    /**
     * @brief constructor
     * @param parent - 父widget
     * @param text - 顯示的文字
     * @param min - 進度的最小值
     * @param max - 進度的最大值
     */
    explicit ProgressWidget(QWidget *parent = nullptr, const QString& text = "", int min = 0, int max = 100);

public slots:
    /// 進度向前一單位
    void progress_advance();
};

#endif // PROGRESSWIDGET_H
