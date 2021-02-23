package com.apd.tema2.factory;

import com.apd.tema2.Main;
import com.apd.tema2.entities.Car;
import com.apd.tema2.entities.Pedestrians;
import com.apd.tema2.entities.ReaderHandler;
import com.apd.tema2.intersections.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;

/**
 * Returneaza sub forma unor clase anonime implementari pentru metoda de citire din fisier.
 */
public class ReaderHandlerFactory {

    public static ReaderHandler getHandler(String handlerType) {
        // simple semaphore intersection
        // max random N cars roundabout (s time to exit each of them)
        // roundabout with exactly one car from each lane simultaneously
        // roundabout with exactly X cars from each lane simultaneously
        // roundabout with at most X cars from each lane simultaneously
        // entering a road without any priority
        // crosswalk activated on at least a number of people (s time to finish all of them)
        // road in maintenance - 1 lane 2 ways, X cars at a time
        // road in maintenance - N lanes 2 ways, X cars at a time
        // railroad blockage for T seconds for all the cars
        // unmarked intersection
        // cars racing
        return switch (handlerType) {
            case "simple_semaphore" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_semaphore");
                }
            };
            case "simple_n_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_n_roundabout");
                    String[] line = br.readLine().split(" ");
                    Main.intersection.no_seconds_in_roundabout = Integer.parseInt(line[1]);
                    Main.intersection.semaphore = new Semaphore(Integer.parseInt(line[0]));
                    Main.intersection.barrier = new CyclicBarrier(Main.carsNo);
                }
            };
            case "simple_strict_1_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_strict_1_car_roundabout");
                    String[] line = br.readLine().split(" ");
                    Main.intersection.no_seconds_in_roundabout = Integer.parseInt(line[1]);
                    Main.intersection.barrier = new CyclicBarrier(Main.carsNo);
                    Main.intersection.semaphores = new Semaphore[Main.carLanes.size()];
                    for (int i = 0; i < Main.carLanes.size(); i++) {
                        Main.intersection.semaphores[i] = new Semaphore(1);
                    }
                }
            };
            case "simple_strict_x_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_strict_x_car_roundabout");
                    String[] line = br.readLine().split(" ");
                    Main.intersection.no_cars_in_roundabout = Integer.parseInt(line[0]);
                    Main.intersection.no_seconds_in_roundabout = Integer.parseInt(line[1]);
                    Main.intersection.barrier = new CyclicBarrier(Main.carsNo);
                    Main.intersection.rBarrier = new CyclicBarrier(Main.carLanes.size() * Integer.parseInt(line[2]));
                    Main.intersection.semaphores = new Semaphore[Main.carLanes.size()];
                    for (int i = 0; i < Main.carLanes.size(); i++) {
                        Main.intersection.semaphores[i] = new Semaphore(Integer.parseInt(line[2]));
                    }
                }
            };
            case "simple_max_x_car_roundabout" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_max_x_car_roundabout");
                    String[] line = br.readLine().split(" ");
                    Main.intersection.no_cars_in_roundabout = Integer.parseInt(line[0]);
                    Main.intersection.no_seconds_in_roundabout = Integer.parseInt(line[1]);
                    Main.intersection.barrier = new CyclicBarrier(Main.carsNo);
                    Main.intersection.rBarrier = new CyclicBarrier(Main.carLanes.size() * Integer.parseInt(line[2]));
                    Main.intersection.semaphores = new Semaphore[Main.carLanes.size()];
                    for (int i = 0; i < Main.carLanes.size(); i++) {
                        Main.intersection.semaphores[i] = new Semaphore(Integer.parseInt(line[2]));
                    }
                }
            };
            case "priority_intersection" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("priority_intersection");
                    String[] line = br.readLine().split(" ");
                    Main.intersection.carsWithPriority = Integer.parseInt(line[0]);
                    Main.intersection.carsWithoutPriority = Integer.parseInt(line[1]);
                    Main.intersection.semaphore = new Semaphore(Main.intersection.carsWithPriority);
                    Main.intersection.queue = new LinkedList<>();
                }
            };
            case "crosswalk" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("crosswalk");
                    String[] line = br.readLine().split(" ");
                    Main.pedestrians = new Pedestrians(Integer.parseInt(line[0]), Integer.parseInt(line[1]));
                    Main.intersection.messages = new ArrayList<>();
                    for (int i = 0; i < Main.carsNo; i++) {
                        Main.intersection.messages.add(new ArrayList<>());
                    }
                }
            };
            case "simple_maintenance" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("simple_maintenance");
                    String[] line = br.readLine().split(" ");
                    Main.intersection.carsPerWay = Integer.parseInt(line[0]);
                    Main.intersection.currentLane = 0;
                    Main.intersection.barrier = new CyclicBarrier(Main.carsNo);
                    Main.intersection.carQ = new LinkedList<>();
                    Main.intersection.carsPassed = 0;
                }
            };
            case "complex_maintenance" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    
                }
            };
            case "railroad" -> new ReaderHandler() {
                @Override
                public void handle(final String handlerType, final BufferedReader br) throws IOException {
                    Main.intersection = IntersectionFactory.getIntersection("railroad");
                    Main.intersection.barrier = new CyclicBarrier(Main.carsNo);
                }
            };
            default -> null;
        };
    }

}
