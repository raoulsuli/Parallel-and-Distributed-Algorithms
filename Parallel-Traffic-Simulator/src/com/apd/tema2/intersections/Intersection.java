package com.apd.tema2.intersections;

import com.apd.tema2.entities.Car;

import java.util.List;
import java.util.Queue;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;

public class Intersection {
    //task2
    public int no_cars_in_roundabout;
    public int no_seconds_in_roundabout;
    public Semaphore semaphore;
    public CyclicBarrier barrier;

    //task3
    public Semaphore[] semaphores;

    //task4
    public CyclicBarrier rBarrier;

    //task6
    public int carsWithoutPriority;
    public int carsWithPriority;
    public Queue<Integer> queue;

    //task7

    public List<List<String>> messages;

    //task8

    public int carsPerWay;
    public int currentLane;
    public List<Car> carQ;
    public int carsPassed;

}
