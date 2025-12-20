from unittest import main, TestCase
from run_udp import run_udp
import socket
import time


class UDPTests(TestCase):
    def test_1(self):
        self.assertEqual(run_udp("01"), "Reply from ESP32: 1")

    def test_2(self):
        self.assertEqual(run_udp("02"), "Reply from ESP32: 2")
    
    def test_3(self):
        self.assertEqual(run_udp("03"), "Reply from ESP32: 3")

    def test_4(self):
        self.assertEqual(run_udp("04"), "Reply from ESP32: 4")

    def test_5(self):
        self.assertEqual(run_udp("05"), "Reply from ESP32: 5")

    def test_6(self):
        self.assertEqual(run_udp("06"), "Reply from ESP32: 6")