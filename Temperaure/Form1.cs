using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Temperature {
    public partial class Form1 : Form {
        public Form1(LocalHistoryDTO localHistoryData, GlobalHistoryDTO globalHistoryData) {
            InitializeComponent();
            localHistoryData.Indoor.ForEach(item => chart1.Series[0].Points.AddXY(item.Hour, item.Value));
            localHistoryData.Outdoor.ForEach(item => chart1.Series[1].Points.AddXY(item.Hour, item.Value));
            localHistoryData.Boiler.ForEach(item => chart1.Series[2].Points.AddXY(item.Hour, item.Value));
            globalHistoryData.Station.ForEach(item => chart1.Series[3].Points.AddXY(item.Hour, item.Value));
            globalHistoryData.Overcast.ForEach(item => chart1.Series[4].Points.AddXY(item.Hour,
                item.Hour.TimeOfDay.Hours > 8 && item.Hour.TimeOfDay.Hours < 16 ? 10 - item.Value : 0));
            globalHistoryData.Wind.ForEach(item => chart1.Series[5].Points.AddXY(item.Hour, item.Value));
            chart1.Titles[0].Text = $"Всего записей: {localHistoryData.Count}. Последняя: {localHistoryData.Last}. ";
            chart1.Series[3].Enabled = false;
            //chart1.Series[4].Enabled = false;
            //chart1.Series[4].Enabled = false;
            //chart1.Series[5].Enabled = false;
        }
    }
}
