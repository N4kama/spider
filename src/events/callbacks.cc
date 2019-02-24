#include "callbacks.hh"
using namespace http;

int total_clients = 0; // Total number of connected clients

/* Accept client requests */
void accept_cb(struct ev_loop* loop, struct ev_io *watcher, int revents)
{
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    int client_sd;
    struct ev_io* w_client = new ev_io(); // = ?

    if (EV_ERROR & revents)
    {
        std::cerr << "got invalid event\n";
        return;
    }

    // Accept client request
    client_sd = accept(watcher->fd,
                       (struct sockaddr*)&client_addr, &client_len);

    if (client_sd < 0)
    {
        std::cerr << "accept error";
        return;
    }

    total_clients++; // Increment total_clients count
    std::cout << "Successfully connected with client.\n";
    std::cout << total_clients << "client(s) connected.\n";

    // Initialize and start watcher to read client requests
    ev_io_init(w_client, read_cb, client_sd, EV_READ);
    ev_io_start(loop, w_client);
}

/* Read client message */
void read_cb(struct ev_loop* loop, struct ev_io *watcher, int revents)
{
    char buffer[4096];
    ssize_t read;

    if (EV_ERROR & revents)
    {
        std::cerr << "got invalid event\n";
        return;
    }

    // Receive message from client socket
    read = recv(watcher->fd, buffer, 4096, 0);

    if (read < 0)
    {
        std::cerr << "read error";
        return;
    }

    if (read == 0)
    {
        // Stop and free watchet if client socket is closing
        ev_io_stop(loop, watcher);
        //free(watcher);
        std::cerr << "peer might closing\n";
        total_clients--; // Decrement total_clients count
        std::cout << total_clients << "client(s) connected.\n";
        return;
    }
    else
    {
        printf("message:%s\n", buffer);
    }

    // Send message bach to the client
    send(watcher->fd, buffer, read, 0);
    bzero(buffer, read);
}