package com.apd.tema2.factory;

import com.apd.tema2.Main;
import com.apd.tema2.entities.*;
import com.apd.tema2.intersections.*;
import com.apd.tema2.utils.Constants;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.List;
import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.concurrent.Semaphore;

import static java.lang.Thread.sleep;

/**
 * Clasa Factory ce returneaza implementari ale InterfaceHandler sub forma unor
 * clase anonime.
 */
public class IntersectionHandlerFactory {

    public static IntersectionHandler getHandler(String handlerType) {
        // simple semaphore intersection
        // max random N cars roundabout (s time to exit each of them)
        // roundabout with exactly one car from each lane simultaneously
        // roundabout with exactly X cars from each lane simultaneously
        // roundabout with at most X cars from each lane simultaneously
        // entering a road without any priority
        // crosswalk activated on at least a number of people (s time to finish all of
        // them)
        // road in maintenance - 2 ways 1 lane each, X cars at a time
        // road in maintenance - 1 way, M out of N lanes are blocked, X cars at a time
        // railroad blockage for s seconds for all the cars
        // unmarked intersection
        // cars racing
        return switch (handlerType) {
            case "simple_semaphore" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    System.out.println("Car " + car.getId() + " has reached the semaphore, now waiting...");
                    try {
                        sleep(car.getWaitingTime());
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                    System.out.println("Car " + car.getId() + " has waited enough, now driving...");
                }
            };
            case "simple_n_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    System.out.println("Car " + car.getId() + " has reached the roundabout, now waiting...");

                    try {
                        Main.intersection.barrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    try {
                        Main.intersection.semaphore.acquire();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has entered the roundabout");

                    try {
                        sleep(Main.intersection.no_seconds_in_roundabout);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has exited the roundabout after " + Main.intersection.no_seconds_in_roundabout / 1000 + " seconds");

                    Main.intersection.semaphore.release();
                }
            };
            case "simple_strict_1_car_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    System.out.println("Car " + car.getId() + " has reached the roundabout");

                    try {
                        Main.intersection.barrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    try {
                        Main.intersection.semaphores[car.getStartDirection()].acquire();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has entered the roundabout from lane " + car.getStartDirection());

                    try {
                        sleep(Main.intersection.no_seconds_in_roundabout);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has exited the roundabout after " + Main.intersection.no_seconds_in_roundabout / 1000 + " seconds");

                    Main.intersection.semaphores[car.getStartDirection()].release();
                }
            };
            case "simple_strict_x_car_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    System.out.println("Car " + car.getId() + " has reached the roundabout, now waiting...");

                    try {
                        Main.intersection.barrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    try {
                        Main.intersection.semaphores[car.getStartDirection()].acquire();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " was selected to enter the roundabout from lane " + car.getStartDirection());

                    try {
                        Main.intersection.rBarrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has entered the roundabout from lane " + car.getStartDirection());

                    try {
                        sleep(Main.intersection.no_seconds_in_roundabout);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has exited the roundabout after " + Main.intersection.no_seconds_in_roundabout / 1000 + " seconds");

                    try {
                        Main.intersection.rBarrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    Main.intersection.semaphores[car.getStartDirection()].release();
                }
            };
            case "simple_max_x_car_roundabout" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    // Get your Intersection instance

                    try {
                        sleep(car.getWaitingTime());
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    } // NU MODIFICATI

                    System.out.println("Car " + car.getId() + " has reached the roundabout from lane " + car.getStartDirection());

                    try {
                        Main.intersection.semaphores[car.getStartDirection()].acquire();
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has entered the roundabout from lane " + car.getStartDirection());

                    try {
                        sleep(Main.intersection.no_seconds_in_roundabout);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " has exited the roundabout after " + Main.intersection.no_seconds_in_roundabout / 1000 + " seconds");

                    Main.intersection.semaphores[car.getStartDirection()].release();
                }
            };
            case "priority_intersection" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    // Get your Intersection instance

                    try {
                        sleep(car.getWaitingTime());
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    } // NU MODIFICATI

                    if (car.getPriority() == 1) {
                        System.out.println("Car " + car.getId() + " with low priority is trying to enter the intersection...");
                        if (Main.intersection.semaphore.availablePermits() == Main.intersection.carsWithPriority) {
                            System.out.println("Car " + car.getId() + " with low priority has entered the intersection");
                        } else {
                            Main.intersection.queue.add(car.getId());
                        }
                    } else {
                        try {
                            Main.intersection.semaphore.acquire();
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        System.out.println("Car " + car.getId() + " with high priority has entered the intersection");
                        try {
                            sleep(2000);
                        } catch (InterruptedException e) {
                            e.printStackTrace();
                        }
                        System.out.println("Car " + car.getId() + " with high priority has exited the intersection");
                        Main.intersection.semaphore.release();
                        if (Main.intersection.semaphore.availablePermits() == Main.intersection.carsWithPriority) {
                            while (!Main.intersection.queue.isEmpty()) {
                                System.out.println("Car " + Main.intersection.queue.remove() + " with low priority has entered the intersection");
                            }
                        }
                    }
                }
            };
            case "crosswalk" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    List<String> lst = Main.intersection.messages.get(car.getId());
                    while (!Main.pedestrians.isFinished()) {
                        if (Main.pedestrians.isPass()) {
                            if (lst.size() == 0 || lst.get(lst.size() - 1).equals("green")) {
                                System.out.println("Car " + car.getId() + " has now red light");
                                lst.add("red");
                            }
                        } else {
                            if (lst.size() == 0 || lst.get(lst.size() - 1).equals("red")) {
                                System.out.println("Car " + car.getId() + " has now green light");
                                lst.add("green");
                            }
                        }
                    }
                    if (Main.pedestrians.isFinished()) {
                        if (lst.get(lst.size() - 1).equals("red")) {
                            System.out.println("Car " + car.getId() + " has now green light");
                        }
                    }
                }
            };
            case "simple_maintenance" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has reached the bottleneck");

                    Main.intersection.carQ.add(car);

                    try {
                        Main.intersection.barrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    List<Car> toRemove = new ArrayList<>();
                    synchronized (Main.intersection.carQ) {
                        for (Car c : Main.intersection.carQ) {
                            if (c.getStartDirection() == Main.intersection.currentLane) {
                                Main.intersection.carsPassed++;
                                System.out.println("Car " + c.getId() + " from side number " + c.getStartDirection() + " has passed the bottleneck");
                                toRemove.add(c);
                            }
                            if (Main.intersection.carsPassed == Main.intersection.carsPerWay) {
                                Main.intersection.carsPassed = 0;
                                Main.intersection.currentLane = (Main.intersection.currentLane == 0) ? 1 : 0;
                            }
                        }
                        for (Car c : toRemove) {
                            Main.intersection.carQ.remove(c);
                        }
                    }
                }
            };
            case "complex_maintenance" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {
                    
                }
            };
            case "railroad" -> new IntersectionHandler() {
                @Override
                public void handle(Car car) {

                    System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has stopped by the railroad");

                    try {
                        Main.intersection.barrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    if (car.getId() == 0) {
                        System.out.println("The train has passed, cars can now proceed");
                    }

                    try {
                        Main.intersection.barrier.await();
                    } catch (InterruptedException | BrokenBarrierException e) {
                        e.printStackTrace();
                    }

                    System.out.println("Car " + car.getId() + " from side number " + car.getStartDirection() + " has started driving");
                }
            };
            default -> null;
        };
    }
}
