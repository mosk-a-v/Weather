google.charts.load('current', { 'packages': ['gauge'] });

var boilerinfo = {};
boilerinfo.chart = {};
boilerinfo.chart.options = {};
boilerinfo.chart.options.temperature = {};
boilerinfo.chart.data = {};
boilerinfo.chart.data.temperature = {};
boilerinfo.chart.data.description = {};
boilerinfo.chart.data.humidity = {};
boilerinfo.chart.data.values = {};

boilerinfo.chart.options.temperature.outdoor = {
    width: 450, height: 150,
    yellowFrom: -30, yellowTo: -15,
    redFrom: 25, redTo: 40,
    majorTicks: 8, minorTicks: 2,
    max: 40, min: -30
};
boilerinfo.chart.options.temperature.indoor = {
    width: 900, height: 150,
    greenFrom: 19, greenTo: 21,
    majorTicks: 4, minorTicks: 5,
    max: 30, min: 15
};
boilerinfo.chart.options.temperature.boiler = {
    width: 450, height: 150,
    greenFrom: 30, greenTo: 50,
    majorTicks: 6, minorTicks: 2,
    max: 60, min: 10
};
boilerinfo.chart.options.humidity = {
    width: 600, height: 150,
    greenFrom: 30, greenTo: 60,
    majorTicks: 8, minorTicks: 2,
    max: 80, min: 10
};
boilerinfo.chart.options.info = {
    width: 300, height: 150
};

boilerinfo.chart.drawOutdoorChart = function () {
    var chart = new google.visualization.Gauge(document.getElementById('outdoor_charts'));
    chart.draw(boilerinfo.chart.data.temperature.outdoor, boilerinfo.chart.options.temperature.outdoor);
    chart.container.children[0].style.width = chart.container.style.width;
}
boilerinfo.chart.drawIndoorChart = function () {
    var chart = new google.visualization.Gauge(document.getElementById('indoor_charts'));
    chart.draw(boilerinfo.chart.data.temperature.indoor, boilerinfo.chart.options.temperature.indoor);
    chart.container.children[0].style.width = chart.container.style.width;
}
boilerinfo.chart.drawBoilerChart = function () {
    var chart = new google.visualization.Gauge(document.getElementById('boiler_charts'));
    chart.draw(boilerinfo.chart.data.temperature.boiler, boilerinfo.chart.options.temperature.boiler);
    chart.container.children[0].style.width = chart.container.style.width;
}
boilerinfo.chart.drawHumidityChart = function () {
    var chart = new google.visualization.Gauge(document.getElementById('humidity_charts'));
    chart.draw(boilerinfo.chart.data.humidity, boilerinfo.chart.options.humidity);
    chart.container.children[0].style.width = chart.container.style.width;
}
boilerinfo.chart.drawInfoChart = function () {
    var chart = new google.visualization.Gauge(document.getElementById('info_charts'));
    chart.draw(boilerinfo.chart.data.info, boilerinfo.chart.options.info);
    chart.container.children[0].style.width = chart.container.style.width;
}

function drawChart() {
    var labels_outdoor = [['Label', 'Value']];
    boilerinfo.chart.data.description.outdoor.forEach(x => labels_outdoor.push([x, 0]));
    boilerinfo.chart.data.temperature.outdoor = google.visualization.arrayToDataTable(labels_outdoor);
    boilerinfo.chart.data.values.outdoor.forEach((item, index) => boilerinfo.chart.data.temperature.outdoor.setValue(index, 1, (item === -100 ? undefined : item)));

    var labels_indoor = [['Label', 'Value']];
    boilerinfo.chart.data.description.indoor.forEach(x => labels_indoor.push([x, 0]));
    boilerinfo.chart.data.temperature.indoor = google.visualization.arrayToDataTable(labels_indoor);
    boilerinfo.chart.data.values.indoor.forEach((item, index) => boilerinfo.chart.data.temperature.indoor.setValue(index, 1, (item === -100 ? undefined : item)));

    var labels_boiler = [['Label', 'Value']];
    boilerinfo.chart.data.description.boiler.forEach(x => labels_boiler.push([x, 0]));
    boilerinfo.chart.data.temperature.boiler = google.visualization.arrayToDataTable(labels_boiler);
    boilerinfo.chart.data.values.boiler.forEach((item, index) => boilerinfo.chart.data.temperature.boiler.setValue(index, 1, (item === -100 ? undefined : item)));
    boilerinfo.chart.data.temperature.boiler.setValue(2, 1, boilerinfo.chart.data.requiredBoiler);

    var labels_humidity = [['Label', 'Value']];
    boilerinfo.chart.data.description.humidity.forEach(x => labels_humidity.push([x, 0]));
    boilerinfo.chart.data.humidity = google.visualization.arrayToDataTable(labels_humidity);
    boilerinfo.chart.data.values.humidity.forEach((item, index) => boilerinfo.chart.data.humidity.setValue(index, 1, (item === -100 ? undefined : item)));

    var labels_info = [['Label', 'Value']];
    boilerinfo.chart.data.description.info.forEach(x => labels_info.push([x, 0]));
    boilerinfo.chart.data.info = google.visualization.arrayToDataTable(labels_info);
    boilerinfo.chart.data.values.info.forEach((item, index) => boilerinfo.chart.data.info.setValue(index, 1, (item === -100 ? undefined : item)));

    boilerinfo.chart.drawOutdoorChart();
    boilerinfo.chart.drawIndoorChart();
    boilerinfo.chart.drawBoilerChart();
    boilerinfo.chart.drawHumidityChart();
    boilerinfo.chart.drawInfoChart();

    $("text:contains('(!)')").parents('svg').css({ 'border-color': 'red', 'border-style': 'solid', 'border-width': 'thin' });
}

google.charts.setOnLoadCallback(drawChart);
