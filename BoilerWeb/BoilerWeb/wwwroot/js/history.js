var boilerinfo = {};
boilerinfo.chart = {};
boilerinfo.chart.options = {};
boilerinfo.chart.options
boilerinfo.chart.data = {};

google.charts.load('current', { 'packages': ['line'] });
google.charts.setOnLoadCallback(drawChart);

function drawChart() {
    var data = new google.visualization.DataTable();

    data.addColumn('datetime', 'Time');
    boilerinfo.chart.data.description.forEach(item => data.addColumn('number', item));

    boilerinfo.chart.data.time.forEach((item, index) => boilerinfo.chart.data.values[index].values.unshift(new Date(Date.parse(item))));
    for (i = 0; i < boilerinfo.chart.data.values.length; i++) {
        for (j = 0; j < boilerinfo.chart.data.values[i].values.length; j++) {
            if (boilerinfo.chart.data.values[i].values[j] === -100) boilerinfo.chart.data.values[i].values[j] = undefined;
        }
    }
    boilerinfo.chart.data.values.forEach(item => item.values.forEach(v => v = (v === -100 ? '' : v)));
    boilerinfo.chart.data.values.forEach(item => data.addRow(item.values));

    var options = {
        width: 900,
        height: 500
    };

    var chart = new google.charts.Line(document.getElementById('linechart_material'));
    chart.draw(data, google.charts.Line.convertOptions(options));
}
