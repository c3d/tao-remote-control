// *****************************************************************************
// remote_control.doxy.h                                           Tao3D project
// *****************************************************************************
//
// File description:
//
//
//
//
//
//
//
//
// *****************************************************************************
// This software is licensed under the GNU General Public License v3
// (C) 2013, Baptiste Soulisse <baptiste.soulisse@taodyne.com>
// (C) 2013, Catherine Burvelle <catherine@taodyne.com>
// (C) 2012-2014,2019, Christophe de Dinechin <christophe@dinechin.org>
// (C) 2012-2013, Jérôme Forissier <jerome@taodyne.com>
// *****************************************************************************
// This file is part of Tao3D
//
// Tao3D is free software: you can r redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Tao3D is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Tao3D, in a file named COPYING.
// If not, see <https://www.gnu.org/licenses/>.
// *****************************************************************************
/**
 * @~english
 * @taomoduledescription{RemoteControl, Remote control}
 * <tt>import RemoteControl</tt> - Start a command-line interface that
 * can be used to send commands to Tao3D.@n
 *
 * The commands are received as raw text over a TCP
 * connection. You can use an application such as telnet to connect to
 * the server.
 *
 * Here is an example:
@code
import RemoteControl

page "Test hook",
    locally
        translatex -200
        draw_frame "Black shape"
        color "black"
        rc_hook
    locally
        translatex 200
        draw_frame "Red shape"
        color "red"
        rc_hook

draw_frame Caption:text ->
    line_width 2
    line_color "black"
    color "white"
    rounded_rectangle 0, 0, 300, 400, 20
    color "black"
    line_width 0
    text_box 0, 0, 300, 400,
        vertical_align_bottom
        align_center
        font "Ubuntu", 24
        text Caption
@endcode
 * Here is the telnet session leading to the screen capture
 * below.
@code
$ telnet localhost 6560
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Tao3D Remote Control Server ready.
#0> xl rotatez 20*sin time ; rectangle 0, 0, 100, 150
#0> @quit
Connection closed by foreign host.
@endcode
 * @image html rc.png "Remote control example"
 *
 * The remote control module allows for potentially complex interactions
 * between Tao3D and another application. The following example
 * illustrates how it is possible to skip to the next page when a predefined
 * condition is reached. It is made of a Tao3D document,
 * <tt>rc2.ddd</tt>, and a script written in Expect, <tt>rc2.expect</tt>. @n
 * The document first has a start page, telling the user to press the down
 * arrow key to start the test. Then the second page shows a 5 second
 * countdown. When the count reaches zero, nothing happens except if the Expect
 * script is active. @n
 * To run this example, first open the document normally, press the down arrow
 * key to show the second page, and notice that the counter indeed reaches zero
 * and remains so. @n
 * Then open the document again, but before skipping to the second page, run
 * the Expect script: @code ./rc2.expect localhost @endcode
 *  Then display page 2. The counter will be decremented as
 * previously, but when it reaches zero you will observe that the Expect script
 * sends a <tt>\@nextp</tt> command, telling the document to skip to page 3.
@code
// rc2.ddd

import RemoteControl

rc_hook

next_page -> if page_number<page_count then goto_page page_name (page_number+1)

my_text_box Body ->
    text_box 0, 0, window_width, window_height,
        font "Orbitron", 48
        align_center
        vertical_align_center
        Body

page "Start",
    my_text_box
        text "↓ to start"


page "Test",
    positive X:integer -> if X > 0 then X else 0
    my_text_box
        remaining -> positive integer (5.0 - page_time)
        text text remaining
        if remaining = 0 then
            rc_writeln "Page 2"
            line_break

page "Done",
    my_text_box
        text "Done!"
        rc_writeln "Page 3"
@endcode
@code
#!/usr/bin/expect
# rc2.expect
spawn telnet [lindex $argv 0] 6560
expect "Page 2"
send "@nextp\r"
expect "Page 3"
@endcode
 * @endtaomoduledescription{RemoteControl}
 *
 * @~french
 * @taomoduledescription{RemoteControl, Contrôle à distance}
 * <tt>import RemoteControl</tt> - Démarre une interface en ligne de commande
 * qui permet de piloter Tao3D et le document en cours.@n
 *
 * Les commandes se présentent
 * sous la forme de text brut, reçu sur une connexion TCP. Vous pouvez
 * utiliser l'application telnet pour vous connecter au serveur.
 *
 * Voici un exemple :
@code
import RemoteControl

page "Test hook",
    locally
        translatex -200
        draw_frame "Black shape"
        color "black"
        rc_hook
    locally
        translatex 200
        draw_frame "Red shape"
        color "red"
        rc_hook

draw_frame Caption:text ->
    line_width 2
    line_color "black"
    color "white"
    rounded_rectangle 0, 0, 300, 400, 20
    color "black"
    line_width 0
    text_box 0, 0, 300, 400,
        vertical_align_bottom
        align_center
        font "Ubuntu", 24
        text Caption
@endcode
 * Voici une copie de la session telnet qui conduit à la capture d'écran
 * qui suit.
@code
$ telnet localhost 6560
Trying 127.0.0.1...
Connected to localhost.
Escape character is '^]'.
Tao3D Remote Control Server ready.
#0> xl rotatez 20*sin time ; rectangle 0, 0, 100, 150
#0> @quit
Connection closed by foreign host.
@endcode
 * @image html rc.png "Remote control example"
 *
 * Le module de contrôle à distance permet des interactions potentiellement
 * complexes entre Tao3D et une autre application. L'exemple qui
 * suit montre comment passer à la page suivante lorsqu'une condition
 * prédéfinie est remplie. Il est composé d'un document Tao3D,
 * <tt>rc2.ddd</tt>, ainsi que d'un script Expect, <tt>rc2.expect</tt>. @n
 * Le document comporte une page de démarrage, qui demande à l'utilisateur
 * d'appuyer sur la touche «flêche vers le bas» pour démarrer le test. Puis,
 * la deuxième page montre un compteur qui décompte 5 secondes. Lorsque ce
 * compteur atteint zéro, rien ne se passe sauf si le script Expect est
 * actif. @n
 * Ouvrez d'abord le document normalement, appuyez sur la flèche pour passer
 * à la deuxième page, et observez aue le compteur ne bouge plus lorsqu'il a
 * atteint zéro. @n
 * Puis ouvrez le document de nouveau, mais avant d'afficher la page 2, lancez
 * le script Expect : @code ./rc2.expect localhost @endcode
 * Affichez la page 2. Le décompte va s'afficher comme précédemment, mais
 * lorsque le compteur atteindra zéro, le script Expect déclenchera le passage
 * à la page suivante en envoyant la commande <tt>\@nextp</tt>.
@code
// rc2.ddd

import RemoteControl

rc_hook

next_page -> if page_number<page_count then goto_page page_name (page_number+1)

my_text_box Body ->
    text_box 0, 0, window_width, window_height,
        font "Orbitron", 48
        align_center
        vertical_align_center
        Body

page "Start",
    my_text_box
        text "↓ to start"


page "Test",
    positive X:integer -> if X > 0 then X else 0
    my_text_box
        remaining -> positive integer (5.0 - page_time)
        text text remaining
        if remaining = 0 then
            rc_writeln "Page 2"
            line_break

page "Done",
    my_text_box
        text "Done!"
        rc_writeln "Page 3"
@endcode
@code
#!/usr/bin/expect
# rc2.expect
spawn telnet [lindex $argv 0] 6560
expect "Page 2"
send "@nextp\r"
expect "Page 3"
@endcode
 * @endtaomoduledescription{RemoteControl}
 * @~
 * @{
 */

/**
 * @~english
 * Executes XL code assigned to a given hook, selects TCP server port.
 * This primitive creates a hook with ID @p id, if it does not exist yet.
 * Any XL command later assigned to the hook by a remote client is executed
 * each time the primitive is evaluated (except if the command is explicitely
 * marked for a unique execution when entered by the client; see the @c help
 * command in the server). Evaluation occurs as defined by
 * the general <em>refresh</em> rules. The rc_hook primitive itself
 * registers for refresh on its own private user event, which is posted
 * each time the hook command is changed. @n
 * rc_hook starts the TCP server if it is not yet running, in which case
 * @p port specifies the TCP port to bind to. The server binds to address
 * @c INADDR_ANY, that is, all local network interfaces that support IPv4. @n
 * @p id is the identifier of the hook an may be chosen freely.
 * @~french
 * Exécute du code XL associé à un hook, précise le port TCP.
 * Cette primitive crée un <em>hook</em> identifié par @p id, si celui-ci
 * n'existe pas déjà. Toute commande XL associée au <em>hook</em> est alors
 * exécutée à chaque fois que la primitive est évaluée (sauf si la commande
 * a été explicitement marquée comme nécessitant une exécution unique par le
 * client ; reférez-vous à la commande @c help du serveur de commandes).
 * L'évaluation obéit aux règles générales de <em>refresh</em> de Tao
 * Presentations. La primitive rc_hook en elle-même demande une ré-exécution
 * du <em>layout</em> sur occurrence d'un évenement privé qui est émis à
 * chaque fois qu'une commande est affectée au <em>hook</em>. @n
 * rc_hook démarre un serveur TCP si ce n'est déjà fait. Dans ce cas, le
 * paramètre @p port précise le port TCP devant être utilisé. Le serveur
 * utilise l'adresse @c INADDR_ANY (qui représente toutes les interfaces
 * réseau local supportant IPv4). @n
 * @p id est l'identifiant du <em>hook</em> et peut être choisi librement.
 * @~
 */
rc_hook(id:integer, port:integer);

/**
 * @~english
 * Executes XL code associated to a given hook.
 * This function is equivalent to:
 * @~french
 * Exécute du code XL associé à un hook
 * Cette fonction est équivalente à:
 * @~
@code rc_hook id, REMOTE_CONTROL_PORT @endcode
 */
rc_hook(id:integer);

/**
 * @~english
 * Executes XL code associated to hook #0.
 * This function is equivalent to:
 * @~french
 * Exécute du code XL associé au hook 0.
 * Cette fonction est équivalente à:
 * @~
@code rc_hook 0, REMOTE_CONTROL_PORT @endcode
 */
rc_hook();

/**
 * @~english
 * Sends a text string to all clients connected to the given hook.
 * This primitive appends a newline character (\\n) to @p msg, then sends the
 * resulting string to all the clients that are currently connected to the
 * remote control server, and that have hook @p id as their current hook. @n
 * The primitive does not create hook @p id if it does not exist, and it
 * does not start a command server either. Therefore you need at least one
 * rc_hook call somewhere for this primitive to be of any use.
 * @~french
 * Envoie une chaîne de caractères à tous les clients connectés à un hook.
 * Un retour à la ligne (\\n) est ajouté à @p msg, puis cette chaîne est
 * envoyée à tous les clients connectés au serveur et dont le <em>hook</em>
 * courant est @p id. @n
 * Cette primitive ne crée pas le <em>hook</em> s'il n'existe pas, et elle ne
 * démarre pas non plus le serveur de commandes. Par conséquent vous devez vous
 * assurer qu'un appel rc_hook est exécuté au moins une fois dans le document
 * avant que rc_writeln puisse être utile.
 * @~
 */
rc_writeln(id:integer, msg:text);

/**
 * @~english
 * Sends a text string to all clients connected to hook #0.
 * This function is equivalent to:
 * @~french
 * Envoie une chaîne de caractères à tous les clients connectés au hook 0.
 * Cette fonction est équivalente à:
 * @~
@code rc_writeln 0, msg @endcode
 */
rc_writeln(mdg:text);


/**
 * @~english
 * The default TCP port for the command server.
 * @~french
 * La valeur par défaut du port TCP sur lequel écoute le serveur de commandes.
 */
REMOTE_CONTROL_PORT = 6560;

/**
 * @}
 */
