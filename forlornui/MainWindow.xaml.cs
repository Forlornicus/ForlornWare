using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Net.Sockets;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using static System.Net.Mime.MediaTypeNames;

namespace forlornui
{
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
        }

        private void set_editor_text(string text)
        {
            editor.Document.Blocks.Clear();
            editor.Document.Blocks.Add(new Paragraph(new Run(text)));
        }

        private void Editor_TextChanged(object sender, TextChangedEventArgs e)
        {
            foreach (Paragraph paragraph in editor.Document.Blocks.OfType<Paragraph>())
            {
                paragraph.LineHeight = 2;
            }
        }

        private string get_editor_text()
        {
            return new TextRange(editor.Document.ContentStart, editor.Document.ContentEnd).Text;
        }

        private void send_script(int port)
        {
            string script = get_editor_text();

            try
            {
                using (TcpClient tcpClient = new TcpClient())
                {
                    tcpClient.Connect("localhost", port);
                    using (NetworkStream stream = tcpClient.GetStream())
                    {
                        byte[] bytes = Encoding.UTF8.GetBytes(script);
                        stream.Write(bytes, 0, bytes.Length);
                        stream.Flush();
                    }
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show("Error: " + ex.Message);
            }
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {
            Process.Start("injector.exe"); // your injector name bcuz pro
        }

        private void Button_Click_1(object sender, RoutedEventArgs e)
        {
            set_editor_text(""); // sets the editors text to nothing which clears the editor
        }

        private void Button_Click_2(object sender, RoutedEventArgs e)
        {
            send_script(2304); // port (2304)
        }
    }
}
