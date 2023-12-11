
#include "ProgressWidget.h"


ProgressWidget::ProgressWidget(QWidget *parent, const QString &text, int min, int max)
    : QWidget(parent), m_layout(new QVBoxLayout(this)),
    m_text(new QLabel(text, this)), m_progress(new QProgressBar(this))
{
    m_layout->addWidget(m_text);
    m_layout->addWidget(m_progress);

    m_progress->setRange(min, max);
    m_progress->setValue(min);
}

void ProgressWidget::progress_advance()
{
    m_progress->setValue(m_progress->value() + 1);
}
