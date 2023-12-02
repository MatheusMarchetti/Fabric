using System;
using System.Reflection;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Documents;
using System.Windows.Media;

namespace editor.Editors
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            Loaded += mainWindow_Loaded;
        }

        private void mainWindow_Loaded(object sender, RoutedEventArgs e)
        {
            Loaded -= mainWindow_Loaded;

            var lpdc = leftPanel.DataContext as DockSpace;
            if (lpdc != null)
            {
                var border = new Border();
                border.Width = 50;
                border.Height = 50;
                border.Background = new SolidColorBrush(Color.FromRgb(255, 0, 0));

                lpdc.Attach(border);
            }

            var rpdc = rightPanel.DataContext as DockSpace;
            if (rpdc != null)
            {
                var border = new Border();
                border.Width = 50;
                border.Height = 50;
                border.Background = new SolidColorBrush(Color.FromRgb(255, 255, 0));

                rpdc.Attach(border);
            }

            var tpdc = topPanel.DataContext as DockSpace;
            if (tpdc != null)
            {
                var border = new Border();
                border.Height = 50;
                border.Width = 50;
                border.Background = new SolidColorBrush(Color.FromRgb(0, 255, 0));

                tpdc.Attach(border);
            }

            var bpdc = bottomPanel.DataContext as DockSpace;
            if (bpdc != null)
            {
                var border = new Border();
                border.Height = 50;
                border.Width = 50;
                border.Background = new SolidColorBrush(Color.FromRgb(0, 255, 255));

                bpdc.Attach(border);
            }

            var ccdc = centerPanel.DataContext as DockSpace;
            if(ccdc != null)
            {
                var border = new Border();
                border.Background = new SolidColorBrush(Color.FromRgb(0, 0, 255));
                border.Height = 50;
                border.Width = 50;
                ccdc.Attach(border);
            }
        }
    }
}
