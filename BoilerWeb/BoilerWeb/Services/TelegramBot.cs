using Microsoft.Extensions.Hosting;
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

        public TelegramBotService(ITelegramBotOptions telegramBotOptions) {
            this.bot = new TelegramBotClient(telegramBotOptions.Token);
            this.telegramBotOptions = telegramBotOptions;
        }
        public async void SendMessage(string message) {
            await bot.SendTextMessageAsync(telegramBotOptions.ChatId, message);
        }

        public Task StartAsync(CancellationToken cancellationToken) {
            return Task.FromResult(0);
        }
        public Task StopAsync(CancellationToken cancellationToken) {
            return Task.FromResult(0);
        }
    }
}
