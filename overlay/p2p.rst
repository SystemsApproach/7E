.. index:: P2P: Peer-to-Peer
.. index:: DHT: Distributed Hash Table

|Overlay|.4 Peer-to-Peer Networks
----------------------------------------

Music-sharing applications like Napster and KaZaA introduced the term
“peer-to-peer” into the popular vernacular. But what exactly does it
mean for a system to be “peer-to-peer”? Certainly in the context of
sharing files it means not having to download music from a central
site, but instead being able to access music files directly from whoever
in the Internet happens to have a copy stored on their computer. More
generally then, we could say that a peer-to-peer network allows a
community of users to pool their resources (content, storage, network
bandwidth, disk bandwidth, CPU), thereby providing access to a larger
archival store, larger video/audio conferences, more complex searches
and computations, and so on than any one user could afford individually.

Quite often, attributes like *decentralized* and *self-organizing* are
mentioned when discussing peer-to-peer networks, meaning that individual
nodes organize themselves into a network without any centralized
coordination. If you think about it, terms like these could be used to
describe the Internet itself. Ironically, Napster was not a
true peer-to-peer system by this definition since it depended on a
central registry of known files, and users had to search this directory
to find what machine offered a particular file. It was only the last
step—actually downloading the file—that took place between machines that
belong to two users, but this is little more than a traditional
client/server transaction. The only difference is that the server is
owned by some other Internet user rather than a large corporation.

So we are back to the original question: What’s interesting about
peer-to-peer networks? One answer is that both the process of locating
an object of interest and the process of downloading that object onto
your local machine happen without your having to contact a centralized
authority, and at the same time the system is able to scale to millions
of nodes. A peer-to-peer system that can accomplish these two tasks in a
decentralized manner turns out to be an overlay network, where the nodes
are those hosts that are willing to share objects of interest (e.g.,
music and other assorted files), and the links (tunnels) connecting
these nodes represent the sequence of machines that you have to visit to
track down the object you want. This description will become clearer
after we look at a popular example: BitTorrent.

BitTorrent is a file-sharing protocol devised by Bram Cohen. It is
based on replicating the file or, rather, replicating segments of the
file, which are called *pieces*. Any particular piece can usually be
downloaded from multiple peers, even if only one peer has the entire
file. The primary benefit of BitTorrent’s replication is avoiding the
bottleneck of having only one source for a file. This is particularly
useful when you consider that any given computer has a limited speed
at which it can serve files over its uplink to the Internet, often
quite a low limit due to the asymmetric nature of most broadband
networks. The beauty of BitTorrent is that replication is a natural
side effect of the downloading process: as soon as a peer downloads a
particular piece, it becomes another source for that piece. The more
peers downloading pieces of the file, the more piece replication
occurs, distributing the load proportionately, and the more total
bandwidth is available to share the file with others. Pieces are
downloaded in random order to avoid a situation where peers find
themselves lacking the same set of pieces.

Each file is shared via its own independent BitTorrent network, called
a *swarm*. (A swarm could potentially share a set of files, but we
describe the single file case for simplicity.) The lifecycle of a
typical swarm is as follows. The swarm starts as a singleton peer with
a complete copy of the file. A node that wants to download the file
joins the swarm, becoming its second member, and begins downloading
pieces of the file from the original peer. In doing so, it becomes
another source for the pieces it has downloaded, even if it has not
yet downloaded the entire file. (In fact, it is common for peers to
leave the swarm once they have completed their downloads, although
they are encouraged to stay longer.) Other nodes join the swarm and
begin downloading pieces from multiple peers, not just the original
peer. See :numref:`Figure %s <fig-bitTorrentSwarm>`.

.. _fig-bitTorrentSwarm:
.. figure:: overlay/figures/f09-29-9780123850591.png
   :width: 500px
   :align: center

   Peers in a BitTorrent swarm download from other peers that may not yet
   have the complete file.

If the file remains in high demand, with a stream of new peers
replacing those who leave the swarm, the swarm could remain active
indefinitely; if not, it could shrink back to include only the
original peer until new peers join the swarm.

Now that we have an overview of BitTorrent, we can ask how requests
are routed to the peers that have a given piece. To make requests, a
would-be downloader must first join the swarm. It starts by
downloading a file containing meta-information about the file and
swarm. The file, which may be easily replicated, is typically
downloaded from a web server and discovered by following links from
Web pages. It contains:

 * Target file’s size
 * Piece size
 * SHA-1 hash values precomputed from each piece
 * URL of the swarm’s *tracker*

A tracker is a server that tracks a swarm’s current membership. We’ll
see in a moment that BitTorrent can be extended to eliminate this
point of centralization, with its attendant potential for bottleneck
or failure.

The would-be downloader then joins the swarm, becoming a peer, by
sending a message to the tracker giving its network address and a peer
ID that it has generated randomly for itself. The message also carries
a SHA-1 hash of the main part of the file, which is used as a
swarm ID.

Let’s call the new peer P. The tracker replies to P with a partial
list of peers giving their IDs and network addresses, and P
establishes connections, over TCP, with some of these peers. Note that
P is directly connected to just a subset of the swarm, although it may
decide to contact additional peers or even request more peers from the
tracker. To establish a BitTorrent connection with a particular peer
after their TCP connection has been established, P sends P’s own peer
ID and swarm ID, and the peer replies with its peer ID and
swarm ID. If the swarm IDs don’t match, or the reply peer ID is not
what P expects, the connection is aborted.

The resulting BitTorrent connection is symmetric: Each end can
download from the other. Each end begins by sending the other a bitmap
reporting which pieces it has, so each peer knows the other’s initial
state. Whenever a downloader (D) finishes downloading another piece,
it sends a message identifying that piece to each of its directly
connected peers, so those peers can update their internal
representation of D’s state. This, finally, is the answer to the
question of how a download request for a piece is routed to a peer
that has the piece, because it means that each peer knows which
directly connected peers have the piece. If D needs a piece that none
of its connections has, it could connect to more or different peers
(it can get more from the tracker) or occupy itself with other pieces
in hopes that some of its connections will obtain the piece from their
connections.

How are objects—in this case, pieces—mapped onto peer nodes? Of course
each peer eventually obtains all the pieces, so the question is really
about which pieces a peer has at a given time before it has all the
pieces or, equivalently, about the order in which a peer downloads
pieces. The answer is that they download pieces in random order, to
keep them from having a strict subset or superset of the pieces of any
of their peers.

The BitTorrent described so far utilizes a central tracker that
constitutes a single point of failure for the swarm and could
potentially be a performance bottleneck. Also, providing a tracker can
be a nuisance for someone who would like to make a file available via
BitTorrent. Newer versions of BitTorrent additionally support
“trackerless” swarms that use consistent hashing, as described in
Section |Overlay|.2. BitTorrent client software that is
trackerless-capable implements not just a BitTorrent peer but also
what we’ll call a *peer finder* (the BitTorrent terminology is simply
*node*), which the peer uses to find peers.

Peer finders form their own overlay network, using their own protocol
over UDP to implement the consistent hash. Furthermore, a peer finder
network includes peer finders whose associated peers belong to
different swarms. In other words, while each swarm forms a distinct
network of BitTorrent peers, a peer finder network instead spans
swarms.

Peer finders randomly generate their own finder IDs, which are the
same size (160 bits) as swarm IDs. Each finder maintains a modest
table containing primarily finders (and their associated peers) whose
IDs are close to its own, plus some finders whose IDs are more
distant. The following algorithm ensures that finders whose IDs are
close to a given swarm ID are likely to know of peers from that swarm;
the algorithm simultaneously provides a way to look them up. When a
finder F needs to find peers from a particular swarm, it sends a
request to the finders in its table whose IDs are close to that
swarm’s ID. If a contacted finder knows of any peers for that swarm,
it replies with their contact information. Otherwise, it replies with
the contact information of the finders in its table that are close to
the swarm, so that F can iteratively query those finders.

After the search is exhausted, because there are no finders closer to
the swarm, F inserts the contact information for itself and its
associated peer into the finders closest to the swarm. The net effect
is that peers for a particular swarm get entered in the tables of the
finders that are close to that swarm.

The above scheme assumes that F is already part of the finder network,
that it already knows how to contact some other finders. This
assumption is true for finder installations that have run previously,
because they are supposed to save information about other finders,
even across executions. If a swarm uses a tracker, its peers are able
to tell their finders about other finders (in a reversal of the peer
and finder roles) because the BitTorrent peer protocol has been
extended to exchange finder contact information. But, how can a newly
installed finder discover other finders? The files for trackerless
swarms include contact information for one or a few finders, instead
of a tracker URL, for just that situation.

An unusual aspect of BitTorrent is that it deals head-on with the
issue of fairness, or good “network citizenship.” Protocols often
depend on the good behavior of individual peers without being able to
enforce it. For example, an unscrupulous Ethernet peer could get
better performance by using a backoff algorithm that is more
aggressive than exponential backoff, or an unscrupulous TCP peer could
get better performance by not cooperating in congestion control.

The good behavior that BitTorrent depends on is peers uploading pieces
to other peers. Since the typical BitTorrent user just wants to
download the file as quickly as possible, there is a temptation to
implement a peer that tries to download all the pieces while doing as
little uploading as possible—this is a bad peer. To discourage bad
behavior, the BitTorrent protocol includes mechanisms that allow peers
to reward or punish each other. If a peer is misbehaving by not nicely
uploading to another peer, the second peer can *choke* the bad peer: It
can decide to stop uploading to the bad peer, at least temporarily,
and send it a message saying so. There is also a message type for
telling a peer that it has been unchoked. The choking mechanism is
also used by a peer to limit the number of its active BitTorrent
connections, to maintain good TCP performance. There are many possible
choking algorithms, and devising a good one is an art.

Because BitTorrent traffic can consume a lot of the uplink bandwidth
on a typical residential Internet connection, it provided inspiration
for work on congestion control algorithms that are less
aggressive than those of TCP, yielding to competing traffic when delay
increases. This led to an experimental RFC on "Low Extra Delay
Background Transport (LEDBAT)" and ongoing research on similar
algorithms.

We conclude by noting that while centrally managed (commercial) music
streaming services are now commonplace, BitTorrent remains in wide
use, with over 170M active users each month. It is used to deliver
everything from music to video, game updates, and software
releases. Popularity is, at least in part, due to its decentralized
design, which is both self-sustaining (more demand naturally results
in more resources) and difficult to shut down (there is no central
authority in charge).

.. admonition:: Further Reading

    `BitTorrent Statistics in 2026
    <https://earthweb.com/bittorrent-statistics/>`__.
    EarthWeb, 2026.
