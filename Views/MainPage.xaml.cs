using System.Windows.Controls;

namespace SaturnLoader.Views
{
    public partial class MainPage : Page
    {
        private readonly MainWindowViewModel _viewModel;

        public MainPage(MainWindowViewModel viewModel)
        {
            InitializeComponent();
            _viewModel = viewModel;
            DataContext = viewModel;
            
            // Navigate to Injector page by default
            NavigateToInjector();
        }

        private void NavigateToInjector()
        {
            ContentFrame.Navigate(new InjectorPage(_viewModel));
            SetActiveButton(InjectorButton);
        }

        private void NavigateToWebsite()
        {
            ContentFrame.Navigate(new WebsitePage());
            SetActiveButton(WebsiteButton);
        }

        private void NavigateToConfig()
        {
            ContentFrame.Navigate(new ConfigPage());
            SetActiveButton(ConfigButton);
        }

        private void NavigateToSupport()
        {
            ContentFrame.Navigate(new SupportPage());
            SetActiveButton(SupportButton);
        }

        private void SetActiveButton(Button activeButton)
        {
            // Reset all buttons
            InjectorButton.Background = System.Windows.Media.Brushes.Transparent;
            WebsiteButton.Background = System.Windows.Media.Brushes.Transparent;
            ConfigButton.Background = System.Windows.Media.Brushes.Transparent;
            SupportButton.Background = System.Windows.Media.Brushes.Transparent;
            
            // Set active button
            activeButton.Background = (System.Windows.Media.Brush)FindResource("SecondaryDarkBrush");
        }

        private void InjectorButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            NavigateToInjector();
        }

        private void WebsiteButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            NavigateToWebsite();
        }

        private void ConfigButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            NavigateToConfig();
        }

        private void SupportButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            NavigateToSupport();
        }
    }
}