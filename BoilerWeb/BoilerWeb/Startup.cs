using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Builder;
using Microsoft.AspNetCore.Hosting;
using Microsoft.Extensions.Configuration;
using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;

namespace BoilerWeb {
    public class Startup {
        public Startup(IConfiguration configuration) {
            Configuration = configuration;
            MapConfiguration();
        }

        public IConfiguration Configuration { get; }

        private void MapConfiguration() {
            MapBrokerHostSettings();
            MapTelegramBotSettings();
        }

        private void MapBrokerHostSettings() {
            var brokerHostSettings = new BrokerHostSettings();
            Configuration.GetSection(nameof(BrokerHostSettings)).Bind(brokerHostSettings);
            AppSettingsProvider.BrokerHostSettings = brokerHostSettings;
        }
        private void MapTelegramBotSettings() {
            var telegramBotSettings = new TelegramBotSettings();
            Configuration.GetSection(nameof(telegramBotSettings)).Bind(telegramBotSettings);
            AppSettingsProvider.TelegramBotSettings = telegramBotSettings;
        }

        // This method gets called by the runtime. Use this method to add services to the container.
        public void ConfigureServices(IServiceCollection services) {
            services.AddTelegramBotHostedService();
            services.AddMqttClientHostedService();
            services.AddSingleton<MqttConsumer>();
            services.AddSingleton<MqttPublisher>();
            services.AddHostedService<MqttConsumer>(serviceProvider => {
                return serviceProvider.GetService<MqttConsumer>();
            });
            services.AddControllersWithViews();
        }

        // This method gets called by the runtime. Use this method to configure the HTTP request pipeline.
        public void Configure(IApplicationBuilder app, IWebHostEnvironment env) {
            if (env.IsDevelopment()) {
                app.UseDeveloperExceptionPage();
            } else {
                app.UseExceptionHandler("/Home/Error");
            }
            app.UseStaticFiles();

            app.UseRouting();

            app.UseAuthorization();

            app.UseEndpoints(endpoints => {
                endpoints.MapControllerRoute(
                    name: "default",
                    pattern: "{controller=Home}/{action=Index}/{id?}");
            });
        }
    }
}
