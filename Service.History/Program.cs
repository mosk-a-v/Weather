using System;
using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace Service.History {
    static class Program {
        static void Main() {
            var service = new HistoryService("http://meteocenter.net/forecast/all.php");
            /* 
             Сервис:
                http://meteocenter.net/27719_fact.htm
             формат ответа:
                Время;Дата;Напр.ветра;Ск.ветра;Видим.;Явл.;Обл.;Форма обл.;Т;Тd;f;Тe;Pmin;dP;Po;Тmin;Tmax;R;s;Сост.почвы;
            */
            var result = service.GetHistoryFrom(2018, 11, 03);
        }
    }
}
