// Compilation: gcc -o 4 4.c -pthread

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <linux/futex.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdatomic.h>

typedef struct {
    atomic_int lock; // Определение структуры спин-блокировки с атомарной переменной
} spinlock_t;

void spinlock_init(spinlock_t *lock) {
    atomic_init(&lock->lock, 0); // Инициализация спин-блокировки
}

void spinlock_lock(spinlock_t *lock) {
    while (atomic_exchange(&lock->lock, 1)) { // Получение спин-блокировки
        while (atomic_load(&lock->lock)) {
            // Spinning
            __asm__ __volatile__("pause"); // Инструкция для паузы в цикле
        }
    }
}

void spinlock_unlock(spinlock_t *lock) {
    atomic_store(&lock->lock, 0); // Освобождение спин-блокировки
}

typedef struct {
    volatile int mutex; // Определение структуры мьютекса на основе Futex
} futex_mutex_t;

void futex_mutex_init(futex_mutex_t *mutex) {
    mutex->mutex = 0; // Инициализация мьютекса
}

void futex_mutex_lock(futex_mutex_t *mutex) {
    while (__sync_lock_test_and_set(&(mutex->mutex), 1)) { // Получение мьютекса на основе Futex
        syscall(SYS_futex, &(mutex->mutex), FUTEX_WAIT_PRIVATE, 1, NULL, NULL, 0); // Ожидание сигнала от Futex
    }
}

void futex_mutex_unlock(futex_mutex_t *mutex) {
    __sync_lock_release(&(mutex->mutex)); // Освобождение мьютекса на основе Futex
    syscall(SYS_futex, &(mutex->mutex), FUTEX_WAKE_PRIVATE, 1, NULL, NULL, 0); // Посылка сигнала Futex
}

int main() {

    // Тестирование спинлока
    spinlock_t spinlock;
    spinlock_init(&spinlock);

    spinlock_lock(&spinlock);
    printf("Spinlock acquired\n");
    sleep(2);
    spinlock_unlock(&spinlock);
    printf("Spinlock released\n");

    futex_mutex_t futex_mutex;
    futex_mutex_init(&futex_mutex);

    // Тестирование мьютекса на основе Futex
    futex_mutex_lock(&futex_mutex);
    printf("Futex mutex acquired\n");
    sleep(2);
    futex_mutex_unlock(&futex_mutex);
    printf("Futex mutex released\n");

    return 0;
}
