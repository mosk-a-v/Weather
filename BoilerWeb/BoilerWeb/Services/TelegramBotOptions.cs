using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;

namespace BoilerWeb {
    public interface ITelegramBotOptions {
        public string Token { get; set; }
        public long ChatId { get; set; }
    }

    public class TelegramBotOptions : ITelegramBotOptions {
        public string Token { get; set; }
        public long ChatId { get; set; }
    }
}
