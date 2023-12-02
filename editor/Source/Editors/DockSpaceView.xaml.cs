using System;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace editor.Editors
{
    public partial class DockSpaceView : UserControl
    {
        public DockSpaceView()
        {
            InitializeComponent();

            Loaded += dockSpaceView_Loaded;
        }

        private void dockSpaceView_Loaded(object sender, RoutedEventArgs e)
        {
            Loaded -= dockSpaceView_Loaded;

            var adornerLayer = AdornerLayer.GetAdornerLayer(contentPresenter);
            adornerLayer.Add(new DockSpaceAdorner(contentPresenter));
        }
    }
}
