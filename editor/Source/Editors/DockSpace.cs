using editor.Core;
using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;

namespace editor.Editors
{
    class DockSpaceAdorner : Adorner
    {
        public DockSpaceAdorner(UIElement adornedElement) : base(adornedElement)
        {

        }

        protected override void OnRender(DrawingContext drawingContext)
        {
            Rect adornedElementRect = new Rect(this.AdornedElement.DesiredSize);

            SolidColorBrush renderBrush = new SolidColorBrush(Colors.Pink);
            renderBrush.Opacity = 0.2;

            Pen renderPen = new Pen(new SolidColorBrush(Colors.Navy), 1.5);
            double renderRadius = 5.0;

            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopLeft, renderRadius, renderRadius);
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.TopRight, renderRadius, renderRadius);
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomLeft, renderRadius, renderRadius);
            drawingContext.DrawEllipse(renderBrush, renderPen, adornedElementRect.BottomRight, renderRadius, renderRadius);
        }
    }

    class DockSpace : ObservableObject
    {
        private FrameworkElement _panel;

        public FrameworkElement Panel
        {
            get => _panel;
            set
            {
                if (_panel != value)
                {
                    _panel = value;
                    OnPropertyChanged();
                }
            }
        }

        public DockSpace()
        {
            _panel = new Border();
        }

        public void Attach(FrameworkElement obj)
        {
            Panel = obj;
        }
    }
}
