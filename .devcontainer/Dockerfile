ARG VARIANT=bookworm
FROM mcr.microsoft.com/vscode/devcontainers/base:${VARIANT}

ENV DEBIAN_FRONTEND=noninteractive
RUN sudo apt-get update \
 && sudo apt-get -y install --no-install-recommends \
      clang \
      python3-venv \
      udev

## Set up udev rules for PlatformIO
RUN curl -fsSL https://raw.githubusercontent.com/platformio/platformio-core/develop/platformio/assets/system/99-platformio-udev.rules | sudo tee /etc/udev/rules.d/99-platformio-udev.rules
RUN service udev restart 
RUN usermod -a -G dialout vscode
RUN usermod -a -G plugdev vscode

# Install PlatformIO CLI
USER vscode
RUN curl -fsSL -o /tmp/get-platformio.py https://raw.githubusercontent.com/platformio/platformio-core-installer/master/get-platformio.py
RUN python3 /tmp/get-platformio.py
RUN echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' | tee -a /home/vscode/.bashrc /home/vscode/.zshrc 
RUN echo 'export PATH="$PATH:$HOME/.platformio/penv/bin"' | sudo tee -a /root/.bashrc /root/.zshrc