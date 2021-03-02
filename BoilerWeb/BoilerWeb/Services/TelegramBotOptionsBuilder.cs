using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BoilerWeb {
    public class TelegramBotOptionsBuilder {
        public IServiceProvider ServiceProvider { get; }
        string token;
        long chatId;

        public TelegramBotOptionsBuilder(IServiceProvider serviceProvider) {
            ServiceProvider = serviceProvider;
        }
        public ITelegramBotOptions Build() {
            return new TelegramBotOptions {
                ChatId = chatId,
                Token = token
            };
        }
        public TelegramBotOptionsBuilder WithApiKey(string key) {
            token = key;
            return this;
        }
        public TelegramBotOptionsBuilder WithChatId(long id) {
            chatId = id;
            return this;
        }
    }
}
