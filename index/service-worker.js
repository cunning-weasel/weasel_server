"use strict";

const registerServiceWorker = async () => {
    if ("serviceWorker" in navigator) {
        try {
            const registration = await navigator.serviceWorker.register("/service-worker.js", {
                scope: "/",
            });
            if (registration.installing) {
                console.log("Service worker installing");
            } else if (registration.waiting) {
                console.log("Service worker installed");
            } else if (registration.active) {
                console.log("Service worker active");
            }
        } catch (error) {
            console.error(`Registration failed with ${error}`);
        }
    }
};
registerServiceWorker();

let db;
let cacheName = "weasel_cache-v1";
const dbName = "weasel_multiplayer-v1";
const storeName = "multiplayer-store";

const cacheAssets = async (assets) => {
    const cache = await caches.open(cacheName);
    await cache.addAll(assets);
};

const putInCache = async (request, response) => {
    const cache = await caches.open(cacheName);
    await cache.put(request, response);
};

const enableNavPreload = async () => {
    if (self.registration) {
        await self.registration.navigationPreload.enable();
    }
};

const deleteOldCaches = async () => {
    const names = await caches.keys();
    await Promise.all(names.map(name => {
        if (name !== cacheName) {
            return caches.delete(name);
        }
    }));
};

// offline first caching strategy
const assetHandler = async (request, preloadResponsePromise) => {
    const cachedRes = await caches.match(request);
    if (cachedRes) {
        return cachedRes;
    }

    const preloadRes = await preloadResponsePromise;
    if (preloadRes) {
        putInCache(request, preloadRes.clone());
        return preloadRes;
    }

    try {
        const networkRes = await fetch(request);
        putInCache(request, networkRes.clone());
        return networkRes;
    } catch (error) {
        console.error(error);
    }
};

self.addEventListener("install", (ev) => {
    self.skipWaiting();
    ev.waitUntil(
        cacheAssets([
            "index.html",
            "index.wasm",
            "index.data",
            // "./manifest.json"
        ]),
    );
});

self.addEventListener("activate", (ev) => {
    ev.waitUntil(
        clients.claim(),    // remove this bad boy?
        enableNavPreload(),
        deleteOldCaches(),
    );
});

self.addEventListener("fetch", (ev) => {
    ev.respondWith(assetHandler(ev.request));
    // IIFE wraps async code as e.waitUnitll expects func returning promise
    ev.waitUntil((async () => {
        const preloadResPromise = ev.preloadResponse;
        if (preloadResPromise) {
            return await preloadResPromise;
        }
    })());
});
