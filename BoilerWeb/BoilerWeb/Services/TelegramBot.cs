using Microsoft.Extensions.Hosting;
using Microsoft.Extensions.Logging;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading;
using System.Threading.Tasks;
using Telegram.Bot;

namespace BoilerWeb {
    public interface ITelegramBotService : IHostedService {
        void SendMessage(string message);
    }

    public class TelegramBotService : ITelegramBotService {
        readonly TelegramBotClient bot;
        readonly ITelegramBotOptions telegramBotOptions;
        private readonly ILogger logger;

        public TelegramBotService(ITelegramBotOptions telegramBotOptions, ILogger<TelegramBotService> loggers) {
            this.bot = new TelegramBotClient(telegramBotOptions.Token);
            this.telegramBotOptions = telegramBotOptions;
            this.logger = loggers;
        }
        public async void SendMessage(string message) {
            try {
                await bot.SendTextMessageAsync(telegramBotOptions.ChatId, message);
            } catch(Exception e) {
                logger.LogError(e, "TelegramBotService. SendMessage");
            }
        }
        public Task StartAsync(CancellationToken cancellationToken)
        {
            return Task.FromResult(0);
        }
        public Task StopAsync(CancellationToken cancellationToken)
        {
            return Task.FromResult(0);
        }
    }
}
