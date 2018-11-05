using System;
using System.Collections.Generic;
using System.Windows.Forms;

namespace Temperaure {
    public partial class Form1 : Form {
        public Form1(List<AverageValue> indoor, List<AverageValue> outdoor, List<AverageValue> boiler, int total, DateTime last) {
            InitializeComponent();
            indoor.ForEach(item => chart1.Series[0].Points.AddXY(item.Hour, item.Value));
            outdoor.ForEach(item => chart1.Series[1].Points.AddXY(item.Hour, item.Value));
            boiler.ForEach(item => chart1.Series[2].Points.AddXY(item.Hour, item.Value));
            chart1.Titles[0].Text = $"Всего записей: {total}. Последняя: {last}. ";
        }
    }
}
