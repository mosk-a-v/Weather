using Microsoft.Extensions.DependencyInjection;
using Microsoft.Extensions.Hosting;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BoilerWeb {
    public static class TelegramBotServiceExtension {
        public static IServiceCollection AddTelegramBotHostedService(this IServiceCollection services) {
            services.AddTelegramBotServiceWithConfig(optionBuilder => {
                var telegramBotSettings = AppSettingsProvider.TelegramBotSettings;
                optionBuilder
                .WithApiKey(telegramBotSettings.Token)
                .WithChatId(telegramBotSettings.ChatId);
            });
            return services;
        }

        private static IServiceCollection AddTelegramBotServiceWithConfig(this IServiceCollection services, Action<TelegramBotOptionsBuilder> configure) {
            services.AddSingleton<ITelegramBotOptions>(serviceProvider => {
                var optionBuilder = new TelegramBotOptionsBuilder(serviceProvider);
                configure(optionBuilder);
                return optionBuilder.Build();
            });
            services.AddSingleton<ITelegramBotService, TelegramBotService>();
            services.AddSingleton<IHostedService>(serviceProvider => {
                return serviceProvider.GetService<ITelegramBotService>();
            });
            services.AddSingleton<TelegramBotServiceProvider>(serviceProvider => {
                var telegramBotService = serviceProvider.GetService<ITelegramBotService>();
                var telegramBotServiceProvider = new TelegramBotServiceProvider(telegramBotService);
                return telegramBotServiceProvider;
            });
            return services;
        }
    }
}
