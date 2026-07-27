<h1>Method to Run Program:</h1>

  <h2>For VS Code:</h2>
  <ol>
    <li>Save <code>run.sh</code>, <code>code.cpp</code>, and <code>README.md</code> in the same folder.</li>
    <li>Run <code>run.sh</code>.</li>
  </ol>

  <h2>For Terminal Running:</h2>
  <ol>
    <li>Go to the folder containing all the above files.</li>
    <li>Make the script executable using <code>chmod +x run.sh</code>.</li>
    <li>Run the script using <code>./run.sh</code>.</li>
  </ol>

<h1>Available Commands:</h1>

  <ul>
    <li>
      <code>ADD_USER &lt;username&gt;</code><br>
      Creates a new user.
    </li>
    <li>
      <code>ADD_FRIEND &lt;user1&gt; &lt;user2&gt;</code><br>
      Makes user1 and user2 friends (both ways).
    </li>
    <li>
      <code>LIST_FRIENDS &lt;username&gt;</code><br>
      Outputs all friends of the user.
    </li>
    <li>
      <code>SUGGEST_FRIENDS &lt;username&gt; &lt;k&gt;</code><br>
      Suggests top k non-friend users with most mutual friends.
    </li>
    <li>
      <code>DEGREES_OF_SEPARATION &lt;user1&gt; &lt;user2&gt;</code><br>
      Prints number of friend-connections in shortest path between the users.
    </li>
    <li>
      <code>ADD_POST &lt;username&gt; &lt;content&gt;</code><br>
      Adds a post for user (content may contain spaces).
    </li>
    <li>
      <code>OUTPUT_POSTS &lt;username&gt; &lt;n&gt;</code><br>
      Gives n most recent posts by the user.
    </li>
    <li>
      <code>EXIT</code><br>
      Exits from the program.
    </li>
  </ul>

<h1>Precaution:</h1>
<ul>
<li>System handles only one command at a time.</li>
<li>All usernames are case-insensitive.</li>
<li>Do not use spaces while naming files.</li>
</ul>