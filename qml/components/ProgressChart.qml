import QtQuick
import TypeBoost 1.0

Item {
    id: root

    property string activeMetric: "wpm"

    onActiveMetricChanged: canvas.requestPaint()

    Connections {
        target: progressModel
        function onModelReset() { canvas.requestPaint() }
    }

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d");
            ctx.clearRect(0, 0, width, height);

            var rowCount = progressModel.rowCount();
            if (rowCount === 0) {
                ctx.fillStyle = Theme.textSecondary;
                ctx.font = "14px " + Theme.fontFamily;
                ctx.textAlign = "center";
                ctx.fillText(qsTr("Нет данных за выбранный период"), width / 2, height / 2);
                return;
            }

            var points = [];
            var roleOffset = 3;
            if (root.activeMetric === "minutes")   roleOffset = 2;
            if (root.activeMetric === "wpm")       roleOffset = 4;
            if (root.activeMetric === "errorRate") roleOffset = 5;

            for (var i = 0; i < rowCount; i++) {
                var idx = progressModel.index(i, 0);
                var dateStr = progressModel.data(idx, Qt.UserRole + 1);
                var val     = progressModel.data(idx, Qt.UserRole + roleOffset);
                points.push({ date: dateStr, value: val });
            }

            var paddingLeft   = 50;
            var paddingRight  = 20;
            var paddingTop    = 20;
            var paddingBottom = 30;

            var graphWidth  = width  - paddingLeft - paddingRight;
            var graphHeight = height - paddingTop  - paddingBottom;

            var maxVal = Math.max.apply(Math, points.map(function(p) { return p.value; }));
            if (maxVal === 0) maxVal = 10;
            maxVal *= 1.15;

            var barSpacing = 6;
            var barWidth   = (graphWidth / rowCount) - barSpacing;
            if (barWidth < 2) barWidth = 2;

            ctx.strokeStyle = Theme.divider;
            ctx.lineWidth   = 1;
            ctx.fillStyle   = Theme.textSecondary;
            ctx.font        = "10px " + Theme.fontFamily;
            ctx.textAlign   = "right";

            for (var j = 0; j <= 3; j++) {
                var gridY   = paddingTop + (graphHeight * (j / 3));
                var gridVal = maxVal * (1 - (j / 3));

                ctx.beginPath();
                ctx.moveTo(paddingLeft, gridY);
                ctx.lineTo(width - paddingRight, gridY);
                ctx.stroke();

                ctx.fillText(Math.round(gridVal), paddingLeft - 8, gridY + 4);
            }

            for (var k = 0; k < rowCount; k++) {
                var p         = points[k];
                var barHeight = (p.value / maxVal) * graphHeight;
                var barX      = paddingLeft + (k * (barWidth + barSpacing)) + (barSpacing / 2);
                var barY      = height - paddingBottom - barHeight;

                if (root.activeMetric === "errorRate") {
                    ctx.fillStyle = p.value > 5 ? Theme.errorColor : Theme.success;
                } else {
                    ctx.fillStyle = Theme.primary;
                }
ctx.fillRect(barX, barY, barWidth, barHeight);

                if (k === 0 || k === Math.floor(rowCount / 2) || k === rowCount - 1) {
                    ctx.fillStyle = Theme.textSecondary;
                    ctx.textAlign = "center";
                    ctx.fillText(p.date, barX + (barWidth / 2), height - paddingBottom + 16);
                }
            }
        }
    }
}
