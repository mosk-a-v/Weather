using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Temperature {
    public partial class Form1 : Form {
        public Form1(List<AverageValue> indoor, List<AverageValue> outdoor, List<AverageValue> boiler, int total, DateTime last,
            List<AverageValue> station, List<AverageValue> overcast, List<AverageValue> wind) {
            InitializeComponent();
            indoor.ForEach(item => chart1.Series[0].Points.AddXY(item.Hour, item.Value));
            outdoor.ForEach(item => chart1.Series[1].Points.AddXY(item.Hour, item.Value));
            boiler.ForEach(item => chart1.Series[2].Points.AddXY(item.Hour, item.Value));
            station.ForEach(item => chart1.Series[3].Points.AddXY(item.Hour, item.Value));
            overcast.ForEach(item => chart1.Series[4].Points.AddXY(item.Hour, 10 - item.Value));
            wind.ForEach(item => chart1.Series[5].Points.AddXY(item.Hour, item.Value));
            chart1.Titles[0].Text = $"Всего записей: {total}. Последняя: {last}. ";
            chart1.Series[3].Enabled = false;
            //chart1.Series[4].Enabled = false;
            //chart1.Series[4].Enabled = false;
            //chart1.Series[5].Enabled = false;
        }
    }
}
