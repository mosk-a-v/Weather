using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BoilerWeb {
    public class TelegramBotServiceProvider {
        public readonly ITelegramBotService TelegramBot;

        public TelegramBotServiceProvider(ITelegramBotService telegramBot) {
            TelegramBot = telegramBot;
        }
    }
}
