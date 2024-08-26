FROM tensorflow/tensorflow:2.14.0-gpu-jupyter

# source: https://code.visualstudio.com/remote/advancedcontainers/add-nonroot-user
ARG USERNAME=user-name-goes-here
ARG USER_UID=1000
ARG USER_GID=$USER_UID

# Create the user
RUN groupadd --gid $USER_GID $USERNAME \
    && useradd --uid $USER_UID --gid $USER_GID -m $USERNAME \
    #
    # [Optional] Add sudo support. Omit if you don't need to install software after connecting.
    && apt-get update \
    && apt-get install -y sudo \
    && echo $USERNAME ALL=\(root\) NOPASSWD:ALL > /etc/sudoers.d/$USERNAME \
    && chmod 0440 /etc/sudoers.d/$USERNAME

# Persist bash history
RUN SNIPPET="export PROMPT_COMMAND='history -a' && export HISTFILE=/commandhistory/.bash_history" \
    && mkdir /commandhistory \
    && touch /commandhistory/.bash_history \
    && chown -R $USERNAME /commandhistory \
    && echo "$SNIPPET" >> "/home/$USERNAME/.bashrc"

# ********************************************************
# * Custom editing                                       *
# ********************************************************
RUN printf "# case insensitivity in bash\nbind 'set completion-ignore-case on'" >> "/home/${USERNAME}/.bashrc" \
    && touch "/home/${USERNAME}/.inputrc" \
    && printf 'set editing-mode vi\n\n# vi settings\n$if mode=vi\n    set keymap vi-insert\n    "jk": vi-movement-mode # remap escape\n$endif\n' >> "/home/${USERNAME}/.inputrc" 

# ********************************************************
# * Anything else you want to do like clean up goes here *
# ********************************************************
RUN pip install tensorflow_datasets pydub mako scipy cmsisdsp \
    && apt-get -y update \
    && apt-get install -y ffmpeg libavcodec-extra xxd

# [Optional] Set the default user. Omit if you want to keep the default as root.
USER $USERNAME

ENTRYPOINT cd ~ && bash