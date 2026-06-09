.. SPDX-FileCopyrightText: 2019 Systems Approach LLC
.. SPDX-FileCopyrightText: 2025 Systems Approach LLC
.. SPDX-License-Identifier: CC-BY-4.0


About This Book
===============

.. image:: https://github.com/SystemsApproach/book/actions/workflows/publish-docs.yml/badge.svg
  :align: left
  :alt: deployment status button
  :target: https://github.com/SystemsApproach/7E/actions/

|

Source for *Computer Networks: A Systems Approach (7th Edition)* is
available on GitHub under terms of the `Creative Commons (CC BY 4.0)
<https://creativecommons.org/licenses/by/4.0>`__ license. The
community is invited to contribute corrections, improvements, updates,
and new material under the same terms. Please reach out to us at
discuss@systemsapproach.org with any feedback or suggestions.

If you make use of this work, the attribution should include the
following information:

|  Title: *Computer Networks: A Systems Approach*
|  Authors: Larry Peterson and Bruce Davie
|  Copyright: Larry Peterson and Bruce Davie, 2026
|  Source: https://github.com/SystemsApproach/7E
|  License: `CC BY  4.0 <https://creativecommons.org/licenses/by/4.0>`__

This book includes content from earlier editions, for which we include
the following attribution:

|  Title: *Computer Networks: A Systems Approach*
|  Authors: Larry Peterson and Bruce Davie
|  Copyright: Elsevier, 2012
|  Source: https://github.com/SystemsApproach/book
|  License: `CC BY  4.0 <https://creativecommons.org/licenses/by/4.0>`__



Read the Book
-------------

This book is part of the `Systems Approach Series
<https://www.systemsapproach.org>`__, with an online version published
at https://book.systemsapproach.org.

To track progress and receive notices about new versions, you can
follow the project on `Mastodon
<https://discuss.systems/@SystemsAppr>`__. To read a running
commentary on how the Internet is evolving, and for updates on our
writing projects, you can sign up for the `Systems Approach newsletter
<https://systemsapproach.org/newsletter/>`__.

Releases and Editions
---------------------

We release ever-changing open source content rather than publish fixed
books, although you can roughly equate v7.0 with a 7th Edition. Read
the :ref:`preface` to find out what’s new in this version. Note that
Morgan Kaufmann (Elsevier) plans to publish the 7th edition of the
textbook based on a fork of v7.0, but going forward, open source
releases found here will not necessarily stay in sync with any future
published editions.

In general, ``main`` contains a coherent and internally consistent
version of the material. (If it were code, the book would build and
run.) New content under development is checked into branches until it
can be merged into ``main`` without breaking self-consistency. The web
version of the book available at https://book.systemsapproach.org is then
continuously generated from ``main``, corresponding to a typical
maintenance release (although we do not bother to tag it as such).

Minor releases (e.g., v7.1) are tagged whenever there is sufficient
new content to justify the effort. This happens infrequently, and is
primarily to create a snapshot so that everyone in a course can know
they are using the same version.

Build the Book
--------------

To build a web-viewable version, you first need to download the
source:

.. code:: shell

   $ mkdir ~/systemsapproach
   $ cd ~/systemsapproach
   $ git clone https://github.com/systemsapproach/7e.git
   $ cd 7e

The build process is stored in the ``Makefile`` and requires Python be
installed. The ``Makefile`` will create a virtualenv (``venv-docs``) which
installs the documentation generation toolset.  You may also need to
install the ``enchant`` C library using your system’s package manager
for the spelling checker to function properly.

To generate HTML in ``_build/html``,  run ``make html``.

To check the formatting of the book, run ``make lint``.

To check spelling, run ``make spelling``. If there are additional
words, names, or acronyms that are correctly spelled but not in the dictionary,
please add them to the ``dict.txt`` file.

To see the other available output formats, run ``make``.

How to Contribute
-----------------

We hope that if you use this material, you are also willing to
contribute back to it. If you are new to open source, you might check
out this `How to Contribute to Open
Source <https://opensource.guide/how-to-contribute/>`__ guide. Among
other things, you’ll learn about posting *Issues* that you’d like to see
addressed, and issuing *Pull Requests* to merge your improvements back
into GitHub.

If you do want to contribute either patches or new material, you will
need to include a `Developer Certificate of Origin
<https://wiki.linuxfoundation.org/dco>`__ stating that you agree to
the terms published at https://developercertificate.org/.


You should also familiarize yourself with the `guidelines for contributing
<https://github.com/SystemsApproach/7E/blob/main/CONTRIBUTING.rst>`__.

If you’d like to contribute and are looking for something that needs
attention, see the current `Project Board
<https://github.com/orgs/SystemsApproach/projects/>`__. We’d also like
to expand the set of topics/chapters beyond the initial set inherited
from the 5th edition, so if you have ideas, we’d love to hear from
you. Send email to ``discuss@systemsapproach.org``, or better yet,
`join the forum
<https://groups.google.com/a/systemsapproach.org/forum/#!forum/discuss>`__.

Finally, in as much as this is an on-going effort, we will try to
record and track our `progress
<https://github.com/SystemsApproach/7E/blob/main/status.rst>`__.  For
now, think of this as a poor-man’s release notes. Additional
information about work-in-progress can be found in the `wiki
<https://github.com/SystemsApproach/7E/wiki>`__.

Join Us
-------

We hope you’ve gotten value out of *Computer Networks: A Systems
Approach* over the years, and we’re eager to have you join us in this
new venture.

| Larry Peterson & Bruce Davie
| June 2026
