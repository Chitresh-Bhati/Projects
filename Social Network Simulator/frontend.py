import streamlit as st
from typing import Optional, List, Tuple
from collections import deque

# -----------------------------
# Backend (AVL implementation)
# -----------------------------
class PostNode:
    def __init__(self, time: int, data: str = ""):
        self.time = time
        self.data = data
        self.left: Optional['PostNode'] = None
        self.right: Optional['PostNode'] = None
        self.height = 1

class AVLTree:
    def __init__(self):
        self.root: Optional[PostNode] = None
    
    def height(self, node: Optional[PostNode]) -> int:
        if not node:
            return 0
        return node.height
    
    def right_rotate(self, y: PostNode) -> PostNode:
        x = y.left
        T2 = x.right
        x.right = y
        y.left = T2
        y.height = max(self.height(y.left), self.height(y.right)) + 1
        x.height = max(self.height(x.left), self.height(x.right)) + 1
        return x
    
    def left_rotate(self, x: PostNode) -> PostNode:
        r = x.right
        x.right = r.left
        r.left = x
        x.height = max(self.height(x.left), self.height(x.right)) + 1
        r.height = max(self.height(r.left), self.height(r.right)) + 1
        return r
    
    def get_balance(self, node: Optional[PostNode]) -> int:
        if not node:
            return 0
        return self.height(node.left) - self.height(node.right)
    
    def insert(self, time: int, data: str):
        self.root = self._insert(self.root, time, data)
    
    def _insert(self, node: Optional[PostNode], time: int, data: str) -> PostNode:
        if node is None:
            return PostNode(time, data)
        
        if time < node.time:
            node.left = self._insert(node.left, time, data)
        elif time > node.time:
            node.right = self._insert(node.right, time, data)
        else:
            node.data += " | " + data
            return node
        
        node.height = 1 + max(self.height(node.left), self.height(node.right))
        balance = self.get_balance(node)
        
        if balance > 1 and time < node.left.time:
            return self.right_rotate(node)
        if balance < -1 and time > node.right.time:
            return self.left_rotate(node)
        if balance > 1 and time > node.left.time:
            node.left = self.left_rotate(node.left)
            return self.right_rotate(node)
        if balance < -1 and time < node.right.time:
            node.right = self.right_rotate(node.right)
            return self.left_rotate(node)
        
        return node
    
    def _rev_inorder(self, node: Optional[PostNode], k: int, out: List[str]) -> int:
        if node is None or k == 0:
            return k
        k = self._rev_inorder(node.right, k, out)
        if k == 0:
            return 0
        out.append(node.data)
        k -= 1
        k = self._rev_inorder(node.left, k, out)
        return k
    
    def get_latest_k(self, k: int) -> List[str]:
        out: List[str] = []
        self._rev_inorder(self.root, k, out)
        return out

class UserNode:
    def __init__(self, username: str):
        self.username = username
        self.friends = set()
        self.posts = AVLTree()

# -----------------------------
# Streamlit session helpers
# -----------------------------
def get_state():
    if 'users' not in st.session_state:
        st.session_state['users'] = {}
    if 'addtime' not in st.session_state:
        st.session_state['addtime'] = 0
    return st.session_state

# -----------------------------
# Core operations
# -----------------------------
def add_user(username: str) -> str:
    state = get_state()
    username = username.strip()
    if not username:
        return "Username cannot be empty"
    if username in state['users']:
        return f"User '{username}' already exists"
    state['users'][username] = UserNode(username)
    return f"User '{username}' added"

def add_friends(user1: str, user2: str) -> str:
    state = get_state()
    user1 = user1.strip()
    user2 = user2.strip()
    if user1 not in state['users']:
        return f"User '{user1}' does not exist"
    if user2 not in state['users']:
        return f"User '{user2}' does not exist"
    if user1 == user2:
        return "Cannot friend yourself"
    state['users'][user1].friends.add(user2)
    state['users'][user2].friends.add(user1)
    return f"'{user1}' and '{user2}' are now friends"

def list_friends(username: str) -> List[str]:
    state = get_state()
    if username not in state['users']:
        return []
    return sorted(list(state['users'][username].friends))

def add_post(username: str, post_content: str) -> str:
    state = get_state()
    username = username.strip()
    if username not in state['users']:
        return f"User '{username}' does not exist"
    t = state['addtime']
    state['addtime'] += 1
    state['users'][username].posts.insert(t, post_content)
    return f"Post added for '{username}' at time {t}"

def output_posts(username: str, n: int) -> List[str]:
    state = get_state()
    username = username.strip()
    if username not in state['users']:
        return []
    return state['users'][username].posts.get_latest_k(n)

def degrees_of_separation(user1: str, user2: str) -> int:
    state = get_state()
    user1 = user1.strip()
    user2 = user2.strip()
    if user1 not in state['users'] or user2 not in state['users']:
        return -1
    if user1 == user2:
        return 0
    
    q = deque()
    visited = set()
    q.append((user1, 0))
    visited.add(user1)
    
    while q:
        cur, dist = q.popleft()
        for fr in state['users'][cur].friends:
            if fr == user2:
                return dist + 1
            if fr not in visited:
                visited.add(fr)
                q.append((fr, dist + 1))
    return -1

def suggest_friends(username: str, k: int) -> List[Tuple[str, int]]:
    state = get_state()
    username = username.strip()
    if username not in state['users']:
        return []
    
    user = state['users'][username]
    suggestions = {}
    
    for friend_name in user.friends:
        if friend_name not in state['users']:
            continue
        for fof in state['users'][friend_name].friends:
            if fof == username:
                continue
            if fof in user.friends:
                continue
            suggestions[fof] = suggestions.get(fof, 0) + 1
    
    sorted_sugg = sorted(suggestions.items(), key=lambda x: (-x[1], x[0]))
    return sorted_sugg[:k]

# -----------------------------
# Streamlit UI - FIXED VERSION
# -----------------------------
st.set_page_config(page_title="Social Network Demo", layout="wide")
st.title("🌐 Social Network — AVL-Based Demo")
st.write("**In-memory social network** with AVL trees for post storage. Add users, connect friends, and explore!")

state = get_state()

# Show current user count
st.info(f"👥 **Total Users:** {len(state['users'])} | **Posts Created:** {state['addtime']}")

# Top row: Add user, add friend, add post
col1, col2, col3 = st.columns(3)

with col1:
    st.subheader("➕ Add User")
    new_user = st.text_input("Username", key="add_user_name", placeholder="Enter username")
    if st.button("Add User", type="primary"):
        msg = add_user(new_user)
        if "added" in msg:
            st.success(msg)
            st.rerun()
        else:
            st.error(msg)

with col2:
    st.subheader("🤝 Add Friendship")
    users_list = [""] + sorted(list(state['users'].keys()))
    u1 = st.selectbox("User 1", options=users_list, key="friend_u1")
    u2 = st.selectbox("User 2", options=users_list, key="friend_u2")
    if st.button("Make Friends", type="primary"):
        if not u1 or not u2:
            st.error("Select both users")
        else:
            msg = add_friends(u1, u2)
            if "now friends" in msg:
                st.success(msg)
                st.rerun()
            else:
                st.error(msg)

with col3:
    st.subheader("📝 Add Post")
    users_list = [""] + sorted(list(state['users'].keys()))
    post_user = st.selectbox("User", options=users_list, key="post_user")
    post_content = st.text_area("Post content", key="post_content", height=80, placeholder="What's on your mind?")
    if st.button("Publish Post", type="primary"):
        if not post_user:
            st.error("Select a user")
        elif not post_content.strip():
            st.error("Post content cannot be empty")
        else:
            msg = add_post(post_user, post_content)
            st.success(msg)
            st.rerun()

st.markdown("---")

# Show all users section
with st.expander("👁️ View All Users", expanded=False):
    if state['users']:
        st.write(", ".join(sorted(list(state['users'].keys()))))
    else:
        st.info("No users yet. Add some above!")

# Reset button
if st.button("🗑️ Reset All Data", type="secondary"):
    state['users'].clear()
    state['addtime'] = 0
    st.warning("All data cleared!")
    st.rerun()

st.markdown("---")

# Middle section: Query operations
tab1, tab2, tab3, tab4 = st.tabs(["👥 Friends", "📰 Posts", "🔍 Suggestions", "📏 Separation"])

with tab1:
    st.subheader("List Friends")
    users_list = [""] + sorted(list(state['users'].keys()))
    lf_user = st.selectbox("Select user", options=users_list, key="lf_user")
    
    if lf_user:
        friends = list_friends(lf_user)
        if friends:
            st.write(f"**{lf_user}'s friends:**")
            for f in friends:
                st.write(f"• {f}")
        else:
            st.info(f"{lf_user} has no friends yet")

with tab2:
    st.subheader("Show Latest Posts")
    users_list = [""] + sorted(list(state['users'].keys()))
    sp_user = st.selectbox("Select user", options=users_list, key="sp_user")
    n_posts = st.slider("Number of posts", min_value=1, max_value=50, value=5, key="n_posts")
    
    if sp_user:
        posts = output_posts(sp_user, n_posts)
        if posts:
            st.write(f"**Latest {len(posts)} post(s) from {sp_user}:**")
            for idx, p in enumerate(posts, 1):
                st.text_area(f"Post {idx}", value=p, height=60, disabled=True, key=f"post_{idx}")
        else:
            st.info(f"{sp_user} has no posts yet")

with tab3:
    st.subheader("Friend Suggestions")
    users_list = [""] + sorted(list(state['users'].keys()))
    sug_user = st.selectbox("User", options=users_list, key="sug_user")
    top_k = st.slider("Top suggestions", min_value=1, max_value=20, value=5, key="top_k")
    
    if sug_user:
        suggestions = suggest_friends(sug_user, top_k)
        if suggestions:
            st.write(f"**Friend suggestions for {sug_user}:**")
            for name, cnt in suggestions:
                st.write(f"• **{name}** — {cnt} mutual friend(s)")
        else:
            st.info(f"No suggestions for {sug_user}")

with tab4:
    st.subheader("Degrees of Separation")
    users_list = [""] + sorted(list(state['users'].keys()))
    col_a, col_b = st.columns(2)
    with col_a:
        ds_u1 = st.selectbox("User 1", options=users_list, key="ds_u1")
    with col_b:
        ds_u2 = st.selectbox("User 2", options=users_list, key="ds_u2")
    
    if ds_u1 and ds_u2:
        dist = degrees_of_separation(ds_u1, ds_u2)
        if dist == -1:
            st.warning("No connection exists")
        else:
            st.success(f"🎯 Degrees of separation: **{dist}**")

st.markdown("---")
st.caption("💡 **Tip:** Data persists during the session but is lost on page refresh.")
