/*
 * Bring-up app: polls IMU + PPG, prints GPS NMEA sentences from uart3.
 * Use `i2c scan i2c1` in the shell first to confirm 0x68 and 0x57.
 */
#include <zephyr/kernel.h>
#include <zephyr/device.h>
#include <zephyr/devicetree.h>
#ifdef CONFIG_SENSOR
#include <zephyr/drivers/sensor.h>
#endif
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>

LOG_MODULE_REGISTER(watch, LOG_LEVEL_INF);

#ifdef CONFIG_SENSOR
static const struct device *const imu = DEVICE_DT_GET(DT_NODELABEL(bmi270));
static const struct device *const ppg = DEVICE_DT_GET(DT_NODELABEL(max30102));
#endif
static const struct device *const gps = DEVICE_DT_GET(DT_NODELABEL(uart3));

static void gps_thread(void *a, void *b, void *c)
{
	char line[100];
	size_t n = 0;
	unsigned char ch;

	while (1) {
		if (uart_poll_in(gps, &ch) == 0) {
			if (ch == '\n' || n >= sizeof(line) - 1) {
				line[n] = '\0';
				if (n > 0) {
					LOG_INF("GPS: %s", line);
				}
				n = 0;
			} else if (ch != '\r') {
				line[n++] = ch;
			}
		} else {
			k_msleep(5);
		}
	}
}
K_THREAD_DEFINE(gps_tid, 1024, gps_thread, NULL, NULL, NULL, 7, 0, 0);

static void print_imu(void)
{
	struct sensor_value a[3], g[3];

	if (sensor_sample_fetch(imu) < 0) {
		LOG_ERR("IMU fetch failed");
		return;
	}
	sensor_channel_get(imu, SENSOR_CHAN_ACCEL_XYZ, a);
	sensor_channel_get(imu, SENSOR_CHAN_GYRO_XYZ, g);
	LOG_INF("accel %d.%06d %d.%06d %d.%06d m/s2",
		a[0].val1, abs(a[0].val2), a[1].val1, abs(a[1].val2),
		a[2].val1, abs(a[2].val2));
	LOG_INF("gyro  %d.%06d %d.%06d %d.%06d rad/s",
		g[0].val1, abs(g[0].val2), g[1].val1, abs(g[1].val2),
		g[2].val1, abs(g[2].val2));
}

static void print_ppg(void)
{
	struct sensor_value red, ir;

	if (sensor_sample_fetch(ppg) < 0) {
		LOG_ERR("PPG fetch failed");
		return;
	}
	sensor_channel_get(ppg, SENSOR_CHAN_RED, &red);
	sensor_channel_get(ppg, SENSOR_CHAN_IR, &ir);
	LOG_INF("ppg red=%d ir=%d", red.val1, ir.val1);
}

int main(void)
{
	if (!device_is_ready(imu)) {
		LOG_ERR("BMI270 not ready (check wiring, 0x68 vs 0x69, VDD)");
	}
	if (!device_is_ready(ppg)) {
		LOG_ERR("MAX30102 not ready (check 1.8V VDD, VLED+, 0x57)");
	}
	if (!device_is_ready(gps)) {
		LOG_ERR("GPS uart not ready");
	}

	while (1) {
		if (device_is_ready(imu)) {
			print_imu();
		}
		if (device_is_ready(ppg)) {
			print_ppg();
		}
		k_msleep(1000);
	}
	return 0;
}
